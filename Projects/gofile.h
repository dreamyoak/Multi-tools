std::string get_server() {
    std::string url = "https://api.gofile.io/getServer";
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << "\n";
        return "";
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        return "";
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
        std::cerr << "Error: HTTP " << http_code << "\n";
        curl_easy_cleanup(curl);
        return "";
    }
    curl_easy_cleanup(curl);
    return response;
}

std::string upload(const std::string& server, const std::string& filepath) {
    std::string url = "https://" + server + ".gofile.io/uploadFile";

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << "\n";
        return "";
    }
    curl_mime* mime;
    curl_mimepart* part;

    mime = curl_mime_init(curl);
    part = curl_mime_addpart(mime);
    curl_mime_name(part, "file");
    curl_mime_filedata(part, filepath.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        curl_mime_free(mime);
        return "";
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
        std::cerr << "Error: HTTP " << http_code << "\n";
        curl_easy_cleanup(curl);
        curl_mime_free(mime);
        return "";
    }
    curl_mime_free(mime);
    curl_easy_cleanup(curl);
    return response;
}

int gofile() {
    std::string filepath;
    while (true) {
        std::cout << "file path >: ";
        std::cin >> filepath;
        std::ifstream file(filepath);
        if(file.good()) {
            break;
        } else {
            system("cls");
            std::cout << "File does not exist!" << "\n";
        }
    }
    std::string response = get_server();
    json res = json::parse(response);
    const std::string server = res["data"]["server"].get<std::string>();
    std::string uploadres = upload(server,filepath);
    json data = json::parse(uploadres);
    const std::string downloadPage = data["data"]["downloadPage"].get<std::string>();
    const std::string guestToken = data["data"]["guestToken"].get<std::string>();
    const std::string code = data["data"]["code"].get<std::string>();
    const std::string parentFolder = data["data"]["parentFolder"].get<std::string>();
    const std::string fileId = data["data"]["fileId"].get<std::string>();
    const std::string fileName = data["data"]["fileName"].get<std::string>();
    const std::string md5 = data["data"]["md5"].get<std::string>();
    system("cls");
    std::ostringstream total;
    total << "Server: " << red << server << reset << "\n" 
          << "Download Page: " << red << downloadPage << reset << "\n" 
          << "Guest Token: " << red << guestToken << reset << "\n" 
          << "Code: " << red << code << reset << "\n" 
          << "Parent Folder: " << red << parentFolder << reset << "\n" 
          << "File ID: " << red << fileId << reset << "\n" 
          << "File Name: " << red << fileName << reset << "\n" 
          << "MD5: " << red << md5 << reset;
    std::cout << total.str() << "\n";
    return 0;
}

