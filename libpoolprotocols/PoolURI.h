#pragma once

#include <string>
//#include <network/uri.hpp>

// A simple URI parser specifically for mining pool endpoints
namespace dev
{

enum class SecureLevel {NONE = 0, TLS12, TLS, ALLOW_SELFSIGNED};
enum class ProtocolFamily {GETWORK = 0, STRATUM};

//class URI : network::uri
class URI
{
public:
	URI() {};
	URI(const std::string uri);

	std::string	Scheme() const;
	std::string	Host() const;
	std::string	Path() const;
	unsigned short	Port() const;
	std::string	User() const;
	std::string	Pswd() const;
	SecureLevel	ProtoSecureLevel() const;
	ProtocolFamily	ProtoFamily() const;
	unsigned	ProtoVersion() const;

	bool		KnownScheme();

	static std::string KnownSchemes(ProtocolFamily family);

private:
	std::string m_host;
	std::string m_path;
	unsigned short m_port;
	std::string m_user;
	std::string m_password;
};

}
