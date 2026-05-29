#include "../PE/Bytes/PEByte.h"
#include "../Utilities/Utilities.h"
#include "../Globals.h"

#include "Networking.h"

bool Networking::ValidateUser(std::string_view szToken)
{
	try
	{
//#ifndef _DEBUG
		// send request
		std::string szHost = xorstr_("");

		for (const auto &part : PEByte::Networking::szHost)
			szHost += part;

        httplib::/*SSLClient*/Client Client(/*szHost*/"127.0.0.1", 8000);
       
		httplib::Result Result = Client.Post(xorstr_("/login"), Globals::Networking::Headers, 
			/*xorstr_("id=1&token=") + std::string(szToken.data())*/
			xorstr_("email=test@mail.com&password=lolkek"), xorstr_("application/x-www-form-urlencoded"));

		int iStatus = Result.value().status;

		if (iStatus != httplib::StatusCode::OK_200)
			throw std::exception(std::to_string(iStatus).c_str());

		std::string szResponse = Result.value().body;

		if (szResponse.empty())
			throw std::exception(xorstr_("Critical #1"));

		if (!nlohmann::json::accept(szResponse))
			throw std::exception(xorstr_("Critical #2"));

		auto jResponse = nlohmann::json::parse(szResponse);

		szResponse.clear();

		if (!jResponse.contains(xorstr_("success")))
			throw std::exception(xorstr_("Critical #3"));

		bool bSuccess = jResponse[xorstr_("success")];

		if (!bSuccess)
		{
			if (!jResponse.contains(xorstr_("message")))
				throw std::exception(xorstr_("Critical #4"));

			throw std::exception(jResponse[xorstr_("message")].get<std::string>().c_str());
		}

		// change json to user info!
		jResponse = jResponse[xorstr_("user")];

		if (!jResponse.contains(xorstr_("username"))|| !jResponse.contains(xorstr_("is_admin")))
			throw std::exception(xorstr_("Critical #5"));

		std::string szUsername = jResponse[xorstr_("username")];
        Globals::UserData::szUserName = szUsername.length() > 7 ? szUsername.substr(0, 7) + "..." : szUsername;		

		szUsername.clear();

		if (jResponse.contains(xorstr_("avatar")))
        {
			std::string szAvatar = base64::from_base64(jResponse[xorstr_("avatar")].get<std::string>());
			Globals::UserData::VecAvatar.insert(Globals::UserData::VecAvatar.end(), szAvatar.begin(), szAvatar.end());
        
			szAvatar.clear();
		}

		if (!jResponse.contains(xorstr_("expires_at")))
			throw std::exception(xorstr_("Critical #6"));

		Globals::UserData::szSubLeft = jResponse[xorstr_("expires_at")];
//#endif

//#if _DEBUG
//        Globals::UserData::szSubLeft = "=(";
//		Globals::UserData::szUserName = "no connect";
//#endif

		// clear unused data 
		jResponse.clear();
	}
	catch (const std::exception& ex)
	{
		Utilities::CreateErrorMessage((xorstr_("Network Error #1: ") + std::string(ex.what())).c_str());
		return false;
	}

	return true;
}

void Networking::Start() 
{
	Globals::Networking::Headers.insert({ xorstr_("X-Requested-With"), xorstr_("XMLHttpRequestLoader") });
	Globals::Networking::Headers.insert({ xorstr_("X-Product"), xorstr_("CK (Crack)") });
}