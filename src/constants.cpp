#include "constants.hpp"
#include <map>
#include <iostream>

const char* http::constants::liveReloadInject = R""""(
<!-- Code Injected By Server -->
<script>
  const url = window.location.origin + "/dev/check-refresh";
  async function fetchStatus() {
    const res = await fetch(url);
    const text = await res.text();
    if (text == "true") {
      window.location.reload();
    }
  }

  (async () => {
    const url = window.location.origin + "/dev/check-refresh";
    try {
      await fetchStatus();
      setInterval(async () => await fetchStatus(), 2500);
    } catch (e) { }
  })();
</script>
)"""";

// source: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
const std::map<int, std::string> http::constants::StatusCodes = {
			{ 100, "Continue" },
			{ 101, "Switching Protocols" },
			{ 102, "Processing" },
			{ 103, "Early Hints" },
			{ 200, "OK" },
			{ 201, "Created" },
			{ 202, "Accepted" },
			{ 203, "Non-Authoritative Information" },
			{ 204, "No Content" },
			{ 205, "Reset Content" },
			{ 206, "Partial Content" },
			{ 207, "Multi-Status" },
			{ 208, "Already Reported" },
			{ 226, "IM Used" },
			{ 300, "Multiple Choices" },
			{ 301, "Moved Permanently" },
			{ 302, "Found" },
			{ 303, "See Other" },
			{ 304, "Not Modified" },
			{ 305, "Use Proxy" },
			{ 306, "unused" },
			{ 307, "Temporary Redirect" },
			{ 308, "Permanent Redirect" },
			{ 400, "Bad Request" },
			{ 401, "Unauthorized" },
			{ 402, "Payment Required" },
			{ 403, "Forbidden" },
			{ 404, "Not Found" },
			{ 405, "Method Not Allowed" },
			{ 406, "Not Acceptable" },
			{ 407, "Proxy Authentication Required" },
			{ 408, "Request Timeout" },
			{ 409, "Conflict" },
			{ 410, "Gone" },
			{ 411, "Length Required" },
			{ 412, "Precondition Failed" },
			{ 413, "Payload Too Large" },
			{ 414, "URI Too Long" },
			{ 415, "Unsupported Media Type" },
			{ 416, "Range Not Satisfiable" },
			{ 417, "Expectation Failed" },
			{ 418, "I'm a teapot" },
			{ 421, "Misdirected Request" },
			{ 422, "Unprocessable Content" },
			{ 423, "Locked" },
			{ 424, "Failed Dependency" },
			{ 425, "Too Early" },
			{ 426, "Upgrade Required" },
			{ 428, "Precondition Required" },
			{ 429, "Too Many Requests" },
			{ 431, "Request Header Fields Too Large" },
			{ 451, "Unavailable For Legal Reasons" },
			{ 500, "Internal Server Error" },
			{ 501, "Not Implemented" },
			{ 502, "Bad Gateway" },
			{ 503, "Service Unavailable" },
			{ 504, "Gateway Timeout" },
			{ 505, "HTTP Version Not Supported" },
			{ 506, "Variant Also Negotiates" },
			{ 507, "Insufficient Storage" },
			{ 508, "Loop Detected" },
			{ 510, "Not Extended" },
			{ 511, "Network Authentication Required" }
};

// source: https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
std::map<std::string, std::string> http::constants::mimeTypes = {
			{".aac", "audio/aac"},
			{".abw", "application/x-abiword"},
			{".apng", "image/apng"},
			{".arc", "application/x-freearc"},
			{".avif", "image/avif"},
			{".avi", "video/x-msvideo"},
			{".azw", "application/vnd.amazon.ebook"},
			{".bin", "application/octet-stream"},
			{".bmp", "image/bmp"},
			{".bz", "application/x-bzip"},
			{".bz2", "application/x-bzip2"},
			{".cda", "application/x-cdf"},
			{".csh", "application/x-csh"},
			{".css", "text/css"},
			{".csv", "text/csv"},
			{".doc", "application/msword"},
			{".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
			{".eot", "application/vnd.ms-fontobject"},
			{".epub", "application/epub+zip"},
			{".gz", "application/gzip"},
			{".gif", "image/gif"},
			{".htm", "text/html"},
			{".html", "text/html"},
			{".ico", "image/vnd.microsoft.icon"},
			{".ics", "text/calendar"},
			{".jar", "application/java-archive"},
			{".jpeg", "image/jpeg"},
			{".js", "text/javascript"},
			{".json", "application/json"},
			{".jsonld", "application/ld+json"},
			{".mid", "audio/midi"},
			{".mjs", "text/javascript"},
			{".mp3", "audio/mpeg"},
			{".mp4", "video/mp4"},
			{".mpeg", "video/mpeg"},
			{".mpkg", "application/vnd.apple.installer+xml"},
			{".odp", "application/vnd.oasis.opendocument.presentation"},
			{".ods", "application/vnd.oasis.opendocument.spreadsheet"},
			{".odt", "application/vnd.oasis.opendocument.text"},
			{".oga", "audio/ogg"},
			{".ogv", "video/ogg"},
			{".ogx", "application/ogg"},
			{".opus", "audio/opus"},
			{".otf", "font/otf"},
			{".png", "image/png"},
			{".pdf", "application/pdf"},
			{".php", "application/x-httpd-php"},
			{".ppt", "application/vnd.ms-powerpoint"},
			{".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
			{".rar", "application/vnd.rar"},
			{".rtf", "application/rtf"},
			{".sh", "application/x-sh"},
			{".svg", "image/svg+xml"},
			{".tar", "application/x-tar"},
			{".tif", "image/tiff"},
			{".ts", "video/mp2t"},
			{".ttf", "font/ttf"},
			{".txt", "text/plain"},
			{".vsd", "application/vnd.visio"},
			{".wav", "audio/wav"},
			{".weba", "audio/webm"},
			{".webm", "video/webm"},
			{".webp", "image/webp"},
			{".woff", "font/woff"},
			{".woff2", "font/woff2"},
			{".xhtml", "application/xhtml+xml"},
			{".xls", "application/vnd.ms-excel"},
			{".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
			{".xml", "application/xml"},
			{".xul", "application/vnd.mozilla.xul+xml"},
			{".zip", "application/zip"},
			{".3gp", "video/3gpp"},
			{".3g2", "video/3gpp2"},
			{".7z", "application/x-7z-compressed"}
};