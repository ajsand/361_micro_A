from http.server import BaseHTTPRequestHandler, HTTPServer
import json, os, hashlib, uuid

HERE       = os.path.dirname(os.path.abspath(__file__))
DATA_FILE  = os.path.join(HERE, "chess_users.json")
PORT      = 5050                 

# ---------- helpers ---------------
def sha256(txt: str) -> str:
    return hashlib.sha256(txt.encode()).hexdigest()

def load_users() -> dict:
    if not os.path.exists(DATA_FILE):
        return {}
    try:
        with open(DATA_FILE, "r", encoding="utf-8") as f:
            return json.load(f)
    except (json.JSONDecodeError, IOError):
        print("Bad JSON – starting with empty user DB")
        return {}

def save_users(users: dict) -> None:
    with open(DATA_FILE, "w", encoding="utf-8") as f:
        json.dump(users, f, indent=2)

users = load_users()   

# ---------- HTTP handler -----------
class AuthHandler(BaseHTTPRequestHandler):
    def _json_body(self):
        length = int(self.headers.get("Content-Length", 0))
        raw = self.rfile.read(length) if length else b"{}"
        try:
            return json.loads(raw)
        except json.JSONDecodeError:
            return {}

    def _send(self, obj, code=200):
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(json.dumps(obj).encode())

    # ---- routes ----
    def do_POST(self):
        data = self._json_body()

        # /register  {username,password}
        if self.path == "/register":
            u, p = data.get("username"), data.get("password")
            if not u or not p:
                return self._send({"status":"error","msg":"missing fields"}, 400)
            if u in users:
                return self._send({"status":"error","msg":"user exists"}, 409)
            users[u] = {
                "passHash": sha256(p),
                "token":    "",
                "stats":    {"games":0,"wins":0}
            }
            save_users(users)
            return self._send({"status":"ok"})

        # /login  {username,password}
        if self.path == "/login":
            u, p = data.get("username"), data.get("password")
            rec = users.get(u)
            if not rec or rec["passHash"] != sha256(p):
                return self._send({"status":"error","msg":"bad credentials"}, 401)
            token = uuid.uuid4().hex
            rec["token"] = token
            save_users(users)
            return self._send({"status":"ok","token":token})

        # /validate  {token}
        if self.path == "/validate":
            tok = data.get("token")
            valid = any(u["token"] == tok for u in users.values())
            return self._send({"status":"ok","valid":valid})

        self._send({"status":"error","msg":"unknown route"}, 404)


if __name__ == "__main__":
    print(f"Auth service running on http://localhost:{PORT}")
    HTTPServer(("0.0.0.0", PORT), AuthHandler).serve_forever()