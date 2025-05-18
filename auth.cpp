#include "auth.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

static size_t writeCB(void* ptr, size_t sz, size_t nmemb, void* userdata)
{
    reinterpret_cast<std::ostringstream*>(userdata)
        ->write(static_cast<char*>(ptr), sz * nmemb);
    return sz * nmemb;
}

/* POST JSON payload, capture JSON reply */
static bool postJson(const std::string& url,
                    const nlohmann::json& payload,
                    nlohmann::json& reply)
{
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string body = payload.dump();

    std::ostringstream resp;
    struct curl_slist* hdrs = nullptr;
    hdrs = curl_slist_append(hdrs, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());  

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCB);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(hdrs);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return false;

    reply = nlohmann::json::parse(resp.str(), nullptr, false);
    return reply.is_object();
}


bool registerUser(const std::string& u, const std::string& p)
{
    nlohmann::json out;
    if (!postJson("http://localhost:5050/register",
                    {{"username", u}, {"password", p}}, out))
        return false;
    return out["status"] == "ok";
}

bool loginUser(const std::string& u, const std::string& p, std::string& token)
{
    nlohmann::json out;
    if (!postJson("http://localhost:5050/login",
                    {{"username", u}, {"password", p}}, out))
        return false;
    if (out["status"] != "ok") return false;
    token = out["token"].get<std::string>();
    return true;
}

bool validateToken(const std::string& tok)
{
    nlohmann::json out;
    if (!postJson("http://localhost:5050/validate",
                    {{"token", tok}}, out))
        return false;
    return out["valid"].get<bool>();
}
