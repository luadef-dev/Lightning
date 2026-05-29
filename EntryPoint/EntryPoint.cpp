#include "../Hooks/Hooks.h"
#include "../Hooks/HooksPathes.h"
#include "../Networking/Networking.h"
#include "../PE/Bytes/PEByte.h"
#include "../PE/PEAllocator.h"
#include "../Utilities/Utilities.h"
#include "../Globals.h"

#include "EntryPoint.h"

void EntryPoint::CreateDirectories()
{
	if (!Utilities::FileManager::CreateDirectoryCustom(PEByte::Pathes::szMainPath->c_str()) ||
		!Utilities::FileManager::CreateDirectoryCustom(Globals::Pathes::szConfigsPath))
		throw std::exception(xorstr_("Failed to create directories #1 #1 or #2"));

	// add default cloud configurations
	if (!Utilities::FileManager::AddDefaultCloudConfigToFolder(xorstr_("3589169401.cfg"),
        Resources::eResources::IDR_CFG_BY_SHIYATO, xorstr_("CFG_BY_SHIYATO")))
		throw std::exception(xorstr_("Failed to add cloud configuration #1 #3"));
}

void EntryPoint::UserData()
{
    nlohmann::json jUserData;

    { // if first start or not founded file =(
        if (!std::filesystem::is_regular_file(Globals::Pathes::szUserFilePath))
        {
            jUserData[xorstr_("username")] = Globals::UserData::szUserName;
            jUserData[xorstr_("subleft")] = Globals::UserData::szSubLeft;

            // reopen new file
            std::ofstream User(Globals::Pathes::szUserFilePath.c_str(), std::ios::out);

            if (!User.is_open())
                throw std::exception(xorstr_("Failed to initialize #2 #2"));

            std::string szDumpedData = jUserData.dump();

            User.write(szDumpedData.data(), szDumpedData.size());
            User.close();

#if _DEBUG
            Utilities::CreateConsoleMessage("saved user data -> " +
                std::to_string(szDumpedData.size()) + "!\n");
#endif

            // clear unused data
            jUserData.clear();
            szDumpedData.clear();
        }
    }

    { // load user info
        std::ifstream UserToRead(Globals::Pathes::szUserFilePath.c_str());

        if (UserToRead.is_open())
        {
            std::string szReadedData = xorstr_("");

            std::getline(UserToRead, szReadedData);
            UserToRead.close();

            if (!nlohmann::json::accept(szReadedData))
                throw std::exception(xorstr_("Failed to initialize #2 #3"));

            jUserData = nlohmann::json::parse(szReadedData);

            // clear unused data
            szReadedData.clear();

            if (!jUserData.contains(xorstr_("username")) || !jUserData.contains(xorstr_("subleft")))
                throw std::exception(xorstr_("Failed to initialize #2 #4"));

            try
            {
                // clear old data ( fix crash )
                Globals::UserData::szUserName.clear();
                Globals::UserData::szSubLeft.clear();

                Globals::UserData::szUserName = jUserData[xorstr_("username")];
                Globals::UserData::szSubLeft = jUserData[xorstr_("subleft")];

                Utilities::FileManager::OpenBinary(Globals::Pathes::szUserAvatarPath, Globals::UserData::VecAvatar);

#if _DEBUG
                Utilities::CreateConsoleMessage("loaded user data -> json: (" + 
                    std::to_string(jUserData.size()) + "), avatar: (" + std::to_string(Globals::UserData::VecAvatar.size()) + ")!\n");
#endif

                // clear unused data
                jUserData.clear();
            }
            catch (...) {
                throw std::exception(xorstr_("Failed to initialize #2 #5"));
            }
        }
    }
}

void EntryPoint::InitPathes()
{
    Globals::szMessagePrefix = xorstr_("t.me/meanwhile_crk");

    std::string szMainPath = blackbone::Utils::WstringToUTF8(HooksPathes::GetNewMainFull());

    { // configs
        std::string szConfigsPrefix;

        for (const auto &part : PEByte::Pathes::szConfigsPrefix)
            szConfigsPrefix += part;

        Globals::Pathes::szConfigsPath = szMainPath + szConfigsPrefix;
    }

    { // crash log
        std::string szCrashLogPrefix;

        for (const auto &part : PEByte::Pathes::szCrashLogPrefix)
            szCrashLogPrefix += part;

        Globals::Pathes::szCrashExceptionFilePath =
            szMainPath + szCrashLogPrefix;
    }

    { // crash top level
        std::string szCrashTopLevelLogPrefix;

        for (const auto &part : PEByte::Pathes::szCrashTopLevelLogPrefix)
            szCrashTopLevelLogPrefix += part;

        Globals::Pathes::szCrashTopLevelExceptionFilePath =
            szMainPath + szCrashTopLevelLogPrefix;
    }

    { // user info
        std::string szUserInfoPrefix;

        for (const auto &part : PEByte::Pathes::szUserInfoPrefix)
            szUserInfoPrefix += part;

        Globals::Pathes::szUserFilePath = szMainPath + szUserInfoPrefix;
    }

    { // avatar
        std::string szUserAvatarPrefix;

        for (const auto &part : PEByte::Pathes::szUserAvatarPrefix)
            szUserAvatarPrefix += part;

        Globals::Pathes::szUserAvatarPath = szMainPath + szUserAvatarPrefix;

        szUserAvatarPrefix.clear();
    }

    szMainPath.clear();
}

void EntryPoint::Start()
{
	try 
	{
#if _DEBUG
        if (!Utilities::CreateConsole())
            return;
#endif

		CreateDirectories();
        UserData();

        Hooks::Start();
        //Networking::Start();

        //// test token for debug $
        //if (!Networking::ValidateUser(xorstr_("1234567890qwertyuiopasdfghjklzxcvbnm")))
        //    throw std::exception(xorstr_("Failed to validate user"));
       
        // check if pidoras aka cracker228 patched networking$$$$$$$$$$$$
        if (Globals::UserData::szUserName.empty() ||
            Globals::UserData::szSubLeft.empty())
            throw std::exception(xorstr_("Data empty bruh :(((("));

        PEAllocator::Start();
        PEAllocator::ReadyCS2();
        PEAllocator::AllocateUserData();
	}
	catch (const std::exception& ex) 
	{
		Utilities::CreateErrorMessage(xorstr_("Critical Error After EntryPoint::Start(): ") + 
			std::string(ex.what()));

        EntryPoint::ShutDown();
	}
}

void EntryPoint::ShutDown()
{
    Hooks::ShutDown();

    PEAllocator::HelperData::bProcess.Detach();

    exit(-1);
}