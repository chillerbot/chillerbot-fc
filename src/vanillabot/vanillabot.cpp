#include <new>

#include <base/system.h>

#include <engine/message.h>
#include <engine/map.h>
#include <engine/storage.h>


#include <engine/shared/compression.h>
#include <engine/shared/network.h>
#include <engine/shared/packer.h>
#include <engine/shared/protocol.h>
#include <engine/shared/config.h>

#include <game/generated/protocol.h>
#include <game/version.h>


#include <engine/client.h> //ChillerDragon
#include <engine/server.h> //ChillerDragon
#include <string.h> //ChillerDragon strncmp linux compiles

#include <stdlib.h>

#include "vanillabot.h"

IEngineMap *pMap;
IStorage *pStorage;

int CFakeClient::SendMsgEx(CMsgPacker *pMsg, int Flags, bool System)
{
	CNetChunk Packet;

	mem_zero(&Packet, sizeof(CNetChunk));

	Packet.m_ClientID = 0;
	Packet.m_pData = pMsg->Data();
	Packet.m_DataSize = pMsg->Size();

	// HACK: modify the message id in the packet and store the system flag
	if (*((unsigned char*)Packet.m_pData) == 1 && System && Packet.m_DataSize == 1)
		dbg_break();

	*((unsigned char*)Packet.m_pData) <<= 1;
	if (System)
		*((unsigned char*)Packet.m_pData) |= 1;

	if (Flags&MSGFLAG_VITAL)
		Packet.m_Flags |= NETSENDFLAG_VITAL;
	if (Flags&MSGFLAG_FLUSH)
		Packet.m_Flags |= NETSENDFLAG_FLUSH;

	if (!(Flags&MSGFLAG_NOSEND))
		m_NetClient.Send(&Packet);
	return 0;
}

void CFakeClient::Disconnect(bool ClearDC)
{
	pMap->Unload();
	m_NetClient.Disconnect(0, ClearDC);

	m_MapdownloadChunk = 0;
	if(m_MapdownloadFile)
		io_close(m_MapdownloadFile);
	m_MapdownloadFile = 0;
	m_MapdownloadCrc = 0;
	m_MapdownloadTotalsize = -1;
	m_MapdownloadAmount = 0;
}

void CFakeClient::Connect(const char *pAddress, bool ClearDC)
{
	char aBuf[512];
	int Port = 8303;

	Disconnect(ClearDC); // dirty disconnect is handeld in network_conn.cpp marked with (ChillerDragon)


	str_copy(m_aServerAddressStr, pAddress, sizeof(m_aServerAddressStr));
	//dbg_msg("fake_client", "connecting to '%s'", m_aServerAddressStr);

	if (net_host_lookup(m_aServerAddressStr, &m_ServerAddress, m_NetClient.NetType()) != 0)
	{
		char aBufMsg[256];
		//dbg_msg("fake_client", "could not find the address of %s, connecting to localhost", aBuf);
		net_host_lookup("localhost", &m_ServerAddress, m_NetClient.NetType());
	}

	if (m_ServerAddress.port == 0)
		m_ServerAddress.port = Port;

	m_NetClient.Connect(&m_ServerAddress);
	SetState(STATE_CONNECTING); //vanilla servers need it
}

void CFakeClient::PumpNetwork()
{
	m_NetClient.Update();

	if(State() != STATE_DEMOPLAYBACK)
	{
		// check for errors
		if(State() != STATE_OFFLINE && State() != STATE_QUITING && m_NetClient.State() == NETSTATE_OFFLINE)
		{
			SetState(STATE_OFFLINE);
			Disconnect();

			dbg_msg("fake_client", "offline error='%s'", m_NetClient.ErrorString());
		}

		//
		if(State() == STATE_CONNECTING && m_NetClient.State() == NETSTATE_ONLINE)
		{
			// we switched to online
			dbg_msg("fake_client", "connected, sending info");

			SetState(STATE_LOADING);
			SendInfo();
		}
	}

	// process packets
	CNetChunk Packet;
	while(m_NetClient.Recv(&Packet))
	{
		if(Packet.m_ClientID == -1)
			ProcessConnlessPacket(&Packet);
		else
			ProcessServerPacket(&Packet);
	}
}

void CFakeClient::SendReady()
{
	CMsgPacker Msg(NETMSG_READY);
	SendMsgEx(&Msg, MSGFLAG_VITAL|MSGFLAG_FLUSH);
}

void CFakeClient::SendInfo()
{
	CMsgPacker Msg(NETMSG_INFO);
	Msg.AddString(GAME_NETVERSION, 128);
	Msg.AddString("", 128);
	SendMsgEx(&Msg, MSGFLAG_VITAL|MSGFLAG_FLUSH);
}

void CFakeClient::SendPlayerInfo(bool start = true)
{
	if (start)
	{
		CNetMsg_Cl_StartInfo Msg;
		Msg.m_pName = "chillerbot.png";
		Msg.m_pClan = "Chilli.*";
		Msg.m_Country = -1;
		Msg.m_pSkin = "greensward";
		Msg.m_UseCustomColor = 0;
		Msg.m_ColorBody = 0;
		Msg.m_ColorFeet = 0;

		CMsgPacker Packer(Msg.MsgID());
		Msg.Pack(&Packer);
		SendMsgEx(&Packer, MSGFLAG_VITAL, false);
	}
	else
	{
		CNetMsg_Cl_ChangeInfo Msg;

		if (m_ClientName == 1)
		{
			Msg.m_pName = "Hood();";
		}
		else if (m_ClientName == 2)
		{
			Msg.m_pName = "Jonny";
		}
		else if (m_ClientName == 3)
		{
			Msg.m_pName = "ID=3";
		}
		else if (m_ClientName == 4)
		{
			Msg.m_pName = "__#";
		}
		else if (m_ClientName == 5)
		{
			Msg.m_pName = "McCP.*";
		}
		else if (m_ClientName == 6)
		{
			Msg.m_pName = "oOoooO";
		}
		else if (m_ClientName == 7)
		{
			Msg.m_pName = "Hello World";
		}
		else if (m_ClientName == 8)
		{
			Msg.m_pName = "nameless tee";
		}
		else if (m_ClientName == 9)
		{
			Msg.m_pName = "brainless tee";
		}
		else if (m_ClientName == 10)
		{
			Msg.m_pName = "ZillyZeHUN";
		}
		else if (m_ClientName == 11)
		{
			Msg.m_pName = "5055";
		}
		else if (m_ClientName == 12)
		{
			Msg.m_pName = "tewelve";
		}
		else if (m_ClientName == 13)
		{
			Msg.m_pName = "3rd teen";
		}
		else if (m_ClientName == 14)
		{
			Msg.m_pName = "Dogger";
		}
		else if (m_ClientName == 15)
		{
			Msg.m_pName = "F R E U D";
		}
		else if (m_ClientName == 16)
		{
			Msg.m_pName = "chcik";
		}
		else if (m_ClientName == 17)
		{
			Msg.m_pName = "Travelor";
		}
		else if (m_ClientName == 18)
		{
			Msg.m_pName = "Kennedy007";
		}
		else if (m_ClientName == 19)
		{
			Msg.m_pName = "quality";
		}
		else if (m_ClientName == 20)
		{
			Msg.m_pName = "GlasFazer";
		}
		else if (m_ClientName == 21)
		{
			Msg.m_pName = "Nolens";
		}
		else if (m_ClientName == 22)
		{
			Msg.m_pName = "freewifi";
		}
		else if (m_ClientName == 23)
		{
			Msg.m_pName = "CallMeMb";
		}
		else if (m_ClientName == 24)
		{
			Msg.m_pName = "Rexx";
		}
		else if (m_ClientName == 25)
		{
			Msg.m_pName = "nudel";
		}
		else
		{
			Msg.m_pName = "chillerbot.png";
		}

		Msg.m_pClan = "Chilli.*";
		Msg.m_Country = -1;
		Msg.m_pSkin = "greensward";
		Msg.m_UseCustomColor = 0;
		Msg.m_ColorBody = 0;
		Msg.m_ColorFeet = 0;

		CMsgPacker Packer(Msg.MsgID());
		Msg.Pack(&Packer);
		SendMsgEx(&Packer, MSGFLAG_VITAL, false);
	}
}

void CFakeClient::SendEnterGame()
{
	CMsgPacker Msg(NETMSG_ENTERGAME);
	SendMsgEx(&Msg, MSGFLAG_VITAL|MSGFLAG_FLUSH);
}

void CFakeClient::EnterGame()
{
	if(State() == STATE_DEMOPLAYBACK)
		return;

	SendEnterGame();
}

const char *CFakeClient::LoadMap(const char *pName, const char *pFilename, unsigned WantedCrc)
{
	static char aErrorMsg[128];

	SetState(STATE_LOADING);

	if(!pMap->Load(pFilename))
	{
		str_format(aErrorMsg, sizeof(aErrorMsg), "map '%s' not found", pFilename);
		return aErrorMsg;
	}

	// get the crc of the map
	if(pMap->Crc() != WantedCrc)
	{
		dbg_msg("fake_client", "map differs from the server. %08x != %08x", pMap->Crc(), WantedCrc);
		pMap->Unload();
		return aErrorMsg;
	}

	dbg_msg("fake_client", "loaded map '%s'", pFilename);

	str_copy(m_aCurrentMap, pName, sizeof(m_aCurrentMap));
	m_CurrentMapCrc = pMap->Crc();

	return 0x0;
}

const char *CFakeClient::LoadMapSearch(const char *pMapName, int WantedCrc)
{
	char aBuf[512];
	const char *pError = 0;

	dbg_msg("fake_client", "loading map, map=%s wanted crc=%08x", pMapName, WantedCrc);
	SetState(STATE_LOADING);

	// try the normal maps folder
	str_format(aBuf, sizeof(aBuf), "maps/%s.map", pMapName);
	pError = LoadMap(pMapName, aBuf, WantedCrc);
	if(!pError)
		return pError;

	// try the downloaded maps
	str_format(aBuf, sizeof(aBuf), "downloadedmaps/%s_%08x.map", pMapName, WantedCrc);
	pError = LoadMap(pMapName, aBuf, WantedCrc);
	if(!pError)
		return pError;

	// search for the map within subfolders
	char aFilename[128];
	str_format(aFilename, sizeof(aFilename), "%s.map", pMapName);
	if(pStorage->FindFile(aFilename, "maps", IStorage::TYPE_ALL, aBuf, sizeof(aBuf)))
		pError = LoadMap(pMapName, aBuf, WantedCrc);

	return pError;
}

void CFakeClient::ProcessServerPacket(CNetChunk *pPacket)
{
	CUnpacker Unpacker;
	Unpacker.Reset(pPacket->m_pData, pPacket->m_DataSize);

	// unpack msgid and system flag
	int Msg = Unpacker.GetInt();
	int Sys = Msg&1;
	Msg >>= 1;

	if(Unpacker.Error())
		return;

	if(Sys)
	{
		if(Msg == NETMSG_MAP_CHANGE)
		{
			const char *pMap = Unpacker.GetString(CUnpacker::SANITIZE_CC|CUnpacker::SKIP_START_WHITESPACES);
			int MapCrc = Unpacker.GetInt();
			int MapSize = Unpacker.GetInt();
			const char *pError = 0;

			if(Unpacker.Error())
				return;

			// no check for valid standard map because I don't give a fuck

			for(int i = 0; pMap[i]; i++) // protect the player from nasty map names
			{
				if(pMap[i] == '/' || pMap[i] == '\\')
					pError = "strange character in map name";
			}

			if(MapSize < 0)
				pError = "invalid map size";

			if(pError)
			{
				dbg_msg("fake_client", "Map change error: %s", pError);
				Disconnect();
			}
			else
			{
				pError = LoadMapSearch(pMap, MapCrc);

				if(!pError)
				{
					dbg_msg("client/network", "loading done");
					SendReady();
					IsMapLoaded++; //ChillerDragon
					m_IsConnected = true; //ChillerDragon
				}
				else
				{
					str_format(m_aMapdownloadFilename, sizeof(m_aMapdownloadFilename), "downloadedmaps/%s_%08x.map", pMap, MapCrc);

					dbg_msg("client/network", "starting to download map to '%s'", m_aMapdownloadFilename);

					m_MapdownloadChunk = 0;
					str_copy(m_aMapdownloadName, pMap, sizeof(m_aMapdownloadName));
					if(m_MapdownloadFile)
						io_close(m_MapdownloadFile);
					m_MapdownloadFile = pStorage->OpenFile(m_aMapdownloadFilename, IOFLAG_WRITE, IStorage::TYPE_SAVE);
					m_MapdownloadCrc = MapCrc;
					m_MapdownloadTotalsize = MapSize;
					m_MapdownloadAmount = 0;

					CMsgPacker Msg(NETMSG_REQUEST_MAP_DATA);
					Msg.AddInt(m_MapdownloadChunk);
					SendMsgEx(&Msg, MSGFLAG_VITAL|MSGFLAG_FLUSH);

					dbg_msg("client/network", "requested chunk %d", m_MapdownloadChunk);
				}
			}
		}
		else if(Msg == NETMSG_MAP_DATA)
		{
			int Last = Unpacker.GetInt();
			int MapCRC = Unpacker.GetInt();
			int Chunk = Unpacker.GetInt();
			int Size = Unpacker.GetInt();
			const unsigned char *pData = Unpacker.GetRaw(Size);

			// check fior errors
			if(Unpacker.Error() || Size <= 0 || MapCRC != m_MapdownloadCrc || Chunk != m_MapdownloadChunk || !m_MapdownloadFile)
				return;

			io_write(m_MapdownloadFile, pData, Size);

			m_MapdownloadAmount += Size;

			if(Last)
			{
				const char *pError;
				dbg_msg("client/network", "download complete, loading map");

				if(m_MapdownloadFile)
					io_close(m_MapdownloadFile);
				m_MapdownloadFile = 0;
				m_MapdownloadAmount = 0;
				m_MapdownloadTotalsize = -1;

				// load map
				pError = LoadMap(m_aMapdownloadName, m_aMapdownloadFilename, m_MapdownloadCrc);
				if(!pError)
				{
					dbg_msg("client/network", "loading done");
					m_IsConnected = true; //ChillerDragon
					SendReady();
				}
				else
				{
					dbg_msg("map data error: %s", pError);
					Disconnect();
				}
			}
			else
			{
				// request new chunk
				m_MapdownloadChunk++;

				CMsgPacker Msg(NETMSG_REQUEST_MAP_DATA);
				Msg.AddInt(m_MapdownloadChunk);
				SendMsgEx(&Msg, MSGFLAG_VITAL|MSGFLAG_FLUSH);

				dbg_msg("client/network", "requested chunk %d", m_MapdownloadChunk);
			}
		}
		else if(Msg == NETMSG_CON_READY)
		{
			SendPlayerInfo();
		}
		else if(Msg == NETMSG_SNAPEMPTY)
		{
			// faggot ddnet anti spoofing protection
			int GameTick = Unpacker.GetInt(); // GamerTick is security token

			if(GameTick > 0)
			{
				//dbg_msg("fake_client", "got security token: %d", GameTick);

				// answer with input packet using security token in the first entry
				CMsgPacker Msg(NETMSG_INPUT);
				Msg.AddInt(GameTick);

				// na we don't need them
				// ddnet only checks first one
				/*Msg.AddInt(1337);
				Msg.AddInt(1337);
				Msg.AddInt(1337);*/

				SendMsgEx(&Msg, MSGFLAG_FLUSH);
			}
		}
		else if (Msg == NETMSG_SNAP || Msg == NETMSG_SNAPSINGLE || Msg == NETMSG_SNAPEMPTY) //added by ChillerDragon
		{
			int GameTick = Unpacker.GetInt();
			//ignored 100 lines of vanilla code which could be imported from client.cpp here
			m_AckGameTick = GameTick;
		}
	}
	else
		OnMessage(Msg, &Unpacker);
}

void CFakeClient::OnMessage(int MsgId, CUnpacker *pUnpacker)
{
	void *pRawMsg = m_NetObjHandler.SecureUnpackMsg(MsgId, pUnpacker);
	if(!pRawMsg)
	{
		dbg_msg("fake_client", "dropped weird message '%s' (%d), failed on '%s'", m_NetObjHandler.GetMsgName(MsgId), MsgId, m_NetObjHandler.FailedMsgOn());
		return;
	}


	if(MsgId == NETMSGTYPE_SV_READYTOENTER)
	{
		EnterGame();
	}
	else if(MsgId == NETMSGTYPE_SV_CHAT)
	{
		//FruchtiHD
		CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;
		dbg_msg("chat", "%d(%d): %s", pMsg->m_ClientID, pMsg->m_Team, pMsg->m_pMessage);
		//char aBuf[256];
		//str_format(aBuf, sizeof(aBuf), "%s: '%s'", pMsg->m_ClientID>-1 && pMsg->m_ClientID<MAX_CLIENTS ? m_aClients[pMsg->m_ClientID].m_aName : "Server", pMsg->m_pMessage);
		//dbg_msg("%s", aBuf);

		if (pMsg->m_ClientID == -1)
		{
			if (!str_comp(pMsg->m_pMessage, "### 'ChillerDragon' is afk"))
			{
				IsAfkChillerDragon = true;
				dbg_msg("afk", "ChillerDragon");
			}
			else if (!str_comp(pMsg->m_pMessage, "### 'ChillerDragon' is back"))
			{
				IsAfkChillerDragon = false;
				dbg_msg("un-afk", "ChillerDragon");
			}
		}

		//ChillerDragon
		if (pMsg->m_ClientID < 0 || pMsg->m_ClientID > MAX_CLIENTS /*|| pMsg->m_ClientID == m_LocalClientID*/) // ADMIN ABUUUUSE!!
			return;

		if (!strncmp(pMsg->m_pMessage, "send_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h", str_length("send_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h"))) //fruchtihd code sscanf
		{
			if (!m_HasID)
			{
				m_CountedID++;
			}
			if (!m_IsOwnSend)
			{
				//Check other versions
				char aVersionFound[6]; //has to be longer than tw msg leng to avoid segmentation faults and bufferoverflows //fixed with %4s reads only 4 bytes
				sscanf(pMsg->m_pMessage, "send_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h_%4s", &aVersionFound);
				if (str_comp(aVersionFound, aBotVersion) > 0)
				{
					m_IsOutdated = true;
					dbg_msg("VERSION", "NEW: %s YOU: %s (OUTDATED!) (send)", aVersionFound, aBotVersion);
				}
				dbg_msg("vSEN_comp", "COMP [ %d ] FOUND [ %s ] ME [ %s ]", str_comp(aVersionFound, aBotVersion), aVersionFound, aBotVersion);
			}
			m_IsOwnSend = false;
		}
		else if (str_find(pMsg->m_pMessage, "request_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h")) //fruchtihd code sscanf
		{
			if (!m_IsOwnRequest)
			{
				SendChat(0, "send_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h_"CHILLERBOT_VERSION);
				m_IsOwnSend = true;

				//Check other versions
				char aVersionFound[6]; //has to be longer than tw msg leng to avoid segmentation faults and bufferoverflows //fixed with %4s reads only 4 bytes
				sscanf(pMsg->m_pMessage, "request_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h_%4s", aVersionFound);

				//dbg_msg("VERSION", "found [ %s ]", aVersionFound);
				if (str_comp(aVersionFound, aBotVersion) > 0)
				{
					m_IsOutdated = true;
					dbg_msg("VERSION", "NEW: %s YOU: %s (OUTDATED!) (request)", aVersionFound, aBotVersion);
				}
				dbg_msg("vREQ_comp", "COMP [ %d ] FOUND [ %s ] ME [ %s ]", str_comp(aVersionFound, aBotVersion), aVersionFound, aBotVersion);
			}

			m_IsOwnRequest = false;
		}


		//Resync IDs
		if (str_find(pMsg->m_pMessage, "sync_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h"))
		{
			if (!m_HasID)
			{
				//compare the hash
				//char aHashFound[5]; //32 000 = 01 234
				//for (int i = 0; i < 5; i++)
				//{
				//	dbg_msg("HASH_debug", "found [ %c ]", pMsg->m_pMessage[47 + i]);
				//	aHashFound[i] = pMsg->m_pMessage[47 + i];
				//}
				char aVersionFound[6]; //has to be longer than tw msg leng to avoid segmentation faults and bufferoverflows //fixed with %4s reads only 4 bytes
				char aHashFound[6]; //has to be longer than tw msg leng to avoid segmentation faults and bufferoverflows //fixed with %4s reads only 4 bytes
				sscanf(pMsg->m_pMessage, "sync_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h_%5s_%4s", aHashFound, aVersionFound);




				dbg_msg("SYNC", "found hash [ %s ] version [ %s ]", aHashFound, aVersionFound);
				if (!str_comp(aHashFound, m_aOwnHash))
				{
					SetID(m_CountedID);
				}

				//TODO: add versions checker here too
			}
			m_CountedID++;
		}


		m_IsIgnoreChat = false;
		bool IsNameTrigger = false;
		if (str_find_nocase(pMsg->m_pMessage, "chillerbot.png") || str_find_nocase(pMsg->m_pMessage, "chillerbotpng") || str_find_nocase(pMsg->m_pMessage, "chillerbot png") || str_find_nocase(pMsg->m_pMessage, ")chillerbot.p")) { IsNameTrigger = true; }

		bool IsExecuteCommand = true;
		if (str_find_nocase(pMsg->m_pMessage, "ID=") && !str_find_nocase(pMsg->m_pMessage, aID_trigger))
		{
			IsExecuteCommand = false;
			dbg_msg("cmd", "ignoring command (ID diff)");
			m_IsIgnoreChat = true;
		}

		if (!m_HasID || m_IsSyncing) return; //dont move or send chat on sync 


		if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Jump();"))
		{
			m_JumpTick = 100;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "MoveLeft();"))
		{
			m_MoveMode = 1;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "MoveRight();"))
		{
			m_MoveMode = 2;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "MoveParty();"))
		{
			m_MoveMode = 3;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "MoveLeft(1);"))
		{
			m_MoveMode = 4;
			m_MoveModeTick = 10;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "MoveLeft(2);"))
		{
			m_MoveMode = 4;
			m_MoveModeTick = 20;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "MoveLeft(3);"))
		{
			m_MoveMode = 4;
			m_MoveModeTick = 30;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "MoveRight(1);"))
		{
			m_MoveMode = 5;
			m_MoveModeTick = 10;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "MoveRight(2);"))
		{
			m_MoveMode = 5;
			m_MoveModeTick = 20;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "MoveRight(3);"))
		{
			m_MoveMode = 5;
			m_MoveModeTick = 30;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "MoveStop();"))
		{
			m_MoveMode = 0;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "HammerFly(true);"))
		{
			m_IsHammerFly = 1;
			m_WantedWeapon = 1;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "HammerFly(false);"))
		{
			m_IsHammerFly = 0;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook();"))
		{
			m_HookMode = 15;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(1);"))
		{
			m_HookMode = 1;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(2);"))
		{
			m_HookMode = 2;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(3);"))
		{
			m_HookMode = 3;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(4);"))
		{
			m_HookMode = 4;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(5);"))
		{
			m_HookMode = 5;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(6);"))
		{
			m_HookMode = 6;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(7);"))
		{
			m_HookMode = 7;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(8);"))
		{
			m_HookMode = 8;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(9);"))
		{
			m_HookMode = 9;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(10);"))
		{
			m_HookMode = 10;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(20);"))
		{
			m_HookMode = 20;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(30);"))
		{
			m_HookMode = 30;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Hook(1337);"))
		{
			m_HookMode = 1337;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Look('left');"))
		{
			m_EyeMode = 1;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Look('right');"))
		{
			m_EyeMode = 2;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Look('up');"))
		{
			m_EyeMode = 3;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Look('down');"))
		{
			m_EyeMode = 4;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Look('right_down');"))
		{
			m_EyeMode = 5;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Look('left_down');"))
		{
			m_EyeMode = 6;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Look('left_up');"))
		{
			m_EyeMode = 7;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Look('right_up');"))
		{
			m_EyeMode = 8;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Look('left_little_up');"))
		{
			m_EyeMode = 9;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Look('right_little_up');"))
		{
			m_EyeMode = 10;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Fire(true);"))
		{
			m_IsFire = true;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Fire(false);"))
		{
			m_IsFire = false;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Weapon(1);")) //hammer
		{
			m_WantedWeapon = 1;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Weapon(2);")) //gun
		{
			m_WantedWeapon = 2;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Weapon(3);")) //shotgun
		{
			m_WantedWeapon = 3;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Weapon(4);")) //rifle
		{
			m_WantedWeapon = 4;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "Weapon(5);")) //chillerdragon
		{
			m_WantedWeapon = 5;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "StopAll();"))
		{
			m_DoMode = 0;
			m_MoveMode = 0;
			m_IsHammerFly = 0;
			m_HookMode = 0;
			m_IsFire = 0;
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Kill();"))
		{
			SendKill();
			m_IsIgnoreChat = true;
		}
		else if (IsExecuteCommand && IsNameTrigger && str_find(pMsg->m_pMessage, "Help();") && ID_trigger == 0)
		{
			SendChat(0, "MoveLeft(); MoveRight(); MoveParty(); StopAll(); Kill(); Hook(); Jump(); Fire(true); Weapon(0); Look('left'); Look('right'); Look('up'); Look('down'); Look('left_down')");
			m_IsIgnoreChat = true;
		}
		else if (IsNameTrigger && str_find(pMsg->m_pMessage, "SayID();")) //DONT ADD NEW COMMANDS BELOW HERE ONLY NON ID SPECIFIC COMMANDS (REST ABOVE Help();)
		{
			if (m_IsOutdated)
			{
				char aBuf[64];
				str_format(aBuf, sizeof(aBuf), "%s [V-%s] (OUTDATED CLIENT)", aID_trigger, CHILLERBOT_VERSION);
				SendChat(0, aBuf);
			}
			else
			{
				char aBuf[64];
				str_format(aBuf, sizeof(aBuf), "%s [V-%s]", aID_trigger, CHILLERBOT_VERSION);
				SendChat(0, aBuf);
			}
			m_IsIgnoreChat = true;
		}
		else if (IsNameTrigger && str_find(pMsg->m_pMessage, "SyncID();"))
		{
			if (!m_IsSyncing)
			{
				m_HasID = false;
				m_IsSyncing = true;
				m_SyncTick = 0;
				m_CountedID = 0;
			}
			m_IsIgnoreChat = true;
		}


		if (m_IsIgnoreChat) return;

		if (ID_trigger != 0) { return; } //only ID=0 sends chat

		bool IsQuestion = false;

		if (str_find_nocase(pMsg->m_pMessage, "?") || str_find_nocase(pMsg->m_pMessage, "what") || str_find_nocase(pMsg->m_pMessage, "how") || str_find_nocase(pMsg->m_pMessage, "wie") || str_find_nocase(pMsg->m_pMessage, "where") || str_find_nocase(pMsg->m_pMessage, "wo") ||
			str_find_nocase(pMsg->m_pMessage, "dont understand") || str_find_nocase(pMsg->m_pMessage, "don't understand") || str_find_nocase(pMsg->m_pMessage, "don't know") || str_find_nocase(pMsg->m_pMessage, "dont know") || str_find_nocase(pMsg->m_pMessage, "idk") ||
			str_find_nocase(pMsg->m_pMessage, "why"))
		{
			IsQuestion = true;
			//dbg_msg("cBug", "found question!");
		}

		bool IsDoWithQuestion = false; 
		if (str_find_nocase(pMsg->m_pMessage, "do with") || str_find_nocase(pMsg->m_pMessage, "what does") || str_find_nocase(pMsg->m_pMessage, "what can i do") || str_find_nocase(pMsg->m_pMessage, "what is") || str_find_nocase(pMsg->m_pMessage, "why") || str_find_nocase(pMsg->m_pMessage, "warum") ||
			str_find_nocase(pMsg->m_pMessage, "buy") || str_find_nocase(pMsg->m_pMessage, "use") || str_find_nocase(pMsg->m_pMessage, "do i need") || str_find_nocase(pMsg->m_pMessage, "needed for") || str_find_nocase(pMsg->m_pMessage, "wozu"))
		{
			IsDoWithQuestion = true;
			dbg_msg("cBug", "found do with question!");
		}

		bool IsHello = false;
		if (str_find_nocase(pMsg->m_pMessage, "yo ") || str_find_nocase(pMsg->m_pMessage, "yo!") || 
			(str_find_nocase(pMsg->m_pMessage, "yo") && !str_find_nocase(pMsg->m_pMessage, "you"))
			|| str_find_nocase(pMsg->m_pMessage, "hello") || str_find_nocase(pMsg->m_pMessage, "hallo") || str_find_nocase(pMsg->m_pMessage, " hi") || str_find_nocase(pMsg->m_pMessage, "ola") || str_find_nocase(pMsg->m_pMessage, "servus") ||
			str_find_nocase(pMsg->m_pMessage, "priviet") || str_find_nocase(pMsg->m_pMessage, "moin") || str_find_nocase(pMsg->m_pMessage, "huhu") || str_find_nocase(pMsg->m_pMessage, "hey"))
		{
			IsHello = true;
			dbg_msg("finder", "found hello");
		}

		if (!str_comp_nocase(pMsg->m_pMessage, "info") || !str_comp_nocase(pMsg->m_pMessage, "!help") || !str_comp_nocase(pMsg->m_pMessage, "chillerbot.png") || str_find_nocase(pMsg->m_pMessage, "Info();"))
		{
			SendChat(0, "[FC-ChillerBot] [V-"CHILLERBOT_VERSION"] Info and support bot by ChillerDragon & FruchtiHD. For more server info check /info or /help.");
		}
		else if (str_find_nocase(pMsg->m_pMessage, "clan") && (str_find_nocase(pMsg->m_pMessage, "chilli") || str_find_nocase(pMsg->m_pMessage, "chili") || str_find_nocase(pMsg->m_pMessage, "chiller")) && IsQuestion)
		{
			//SendChat(0, "All infos about the Chilli.* clan at www.chillerdragon.weebly.com");

			int rand_index = rand() % 11;
			char aChilliMsg[11][128] = { 
				"All infos about the Chilli.* clan at www.chillerdragon.weebly.com", 
				"Basically everybody can join Chilli.* as long as he uses the skin greensward", 
				"Use the clanskin greensward and feel free to stay in clan as long as you use the skin.", 
				"The first clanrule is to use the skin greensward more infos at www.chillerdragon.weebly.com", 
				"As long as you use the clanskin greensward you can stay in Chilli.*",
				"The Chilli.* clan is an funclan! All people with greensward skin can join.", 
				"First rule: use clanskin greensward. If you want to use another skin remove clantag real quick.", 
				"As long as you use the clanskin greensward you are a member of the Chilli.* clan!", 
				"All tees with greensward skin can join the funclan. But keep the skin as long as you keep the clantag.",
				"feel free to join if you use the Chilli.* clanskin (greensward) can be found in ddnet.tw client for example.",
				"If you want to join Chilli.* make sure to read the infos on www.chillerdragon.weebly.com"
				};
			SendChat(0, aChilliMsg[rand_index]);
		}
		else if (str_find_nocase(pMsg->m_pMessage, "chillerdragon") && IsAfkChillerDragon)
		{
			SendChat(0, "ChillerDragon is afk. But maybe i can help you c:");
		}
		else if (IsNameTrigger && IsHello)
		{
			int rand_index = rand() % 16;
			char aHelloMsg[16][16] = {"hello", "hi", "yo", "priviet", "olla", "huh", "huhu", "hu", "yoo", "ay", "sup", "wazzup", "yoo mate", "yo yo", "ello", "wena"};
			SendChat(0, aHelloMsg[rand_index]);
		}
		else if (IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "quiz"))
		{
			SendChat(0, "I am not a quiz bot. I am a Q&A bot. Ask me a question and i'll answer.");
		}
		else if (IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "apt-get install"))
		{
			SendChat(0, "installing...");
		}
		else if (IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "flag"))
		{
			SendChat(0, "CAPTURE THE FLAG!!!!");
		}
		else if (IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "why"))
		{
			SendChat(0, "because of reasons....");
		}
		else if (IsNameTrigger && str_find_nocase(pMsg->m_pMessage, "joke") && (str_find_nocase(pMsg->m_pMessage, "tell ") || str_find_nocase(pMsg->m_pMessage, "tell a") || str_find_nocase(pMsg->m_pMessage, "!") || str_find_nocase(pMsg->m_pMessage, "tell us") || str_find_nocase(pMsg->m_pMessage, "tell me") || str_find_nocase(pMsg->m_pMessage, "say me") || str_find_nocase(pMsg->m_pMessage, "can i hear") || str_find_nocase(pMsg->m_pMessage, "could i hear") || str_find_nocase(pMsg->m_pMessage, "give me") || str_find_nocase(pMsg->m_pMessage, "drop")))
		{
			int rand_index = rand() % 32;
			char aJokeMsg[32][256] = {
			"you are a joke xxxxd", //0
			"My friends say there's a gay guy in our circle of friends... I really hope it's Todd, he's cute.", //1
			"Time flies like an arrow, fruit flies like a banana", //2
			"What's orange and sounds like a parrot?          A carrot.", //3
			"What do you call a magic dog?      A Labracadabrador.", //4
			"What's the difference between a good joke and a bad joke timing.", //5
			"My friend says to me: 'what rhymes with orange' I said: 'no it doesn't'", //6
			"Why did the old man fall in the well? Because he couldn't see that well.", //7
			"A blind man walks into a bar. And a table. And a chair.", //8
			"I used to be addicted to soap, but now I'm clean...", //9
			"How did the hipster burn his mouth?                  He ate the pizza before it was cool.", //10
			"What's the difference between in-laws and outlaws? Outlaws are wanted.", //11
			"Why do you never see hippopotamuses hiding in trees? They are really good at it.", //12
			"What is Beethoven's favorite fruit? A - NA - NA - NAAAAAAAA", //13
			"An Irish man walks out of a bar.", //14
			"Why doesn't anyone eat Wookie meat?                      It's Chewie.", //15
			"How do you make an octopus laugh? Ten tickles", //16
			"The shovel was a groundbreaking invention.", //17
			"24 hour banking? I don't have time for that.", //18
			"What's red and bad for your teeth? A brick", //19
			"If two vegans had an argument... is it still beef?", //20
			"if apple made a car would it have windows?", //21
			"If an illegal immigrant fought a child molester, would it be Alien vs Predator?", //22
			"What kind of shoes does a pedophile wear?    White vans", //23
			"what I if told you... you that read wrong", //24
			"I love the Peter Pan joke....              because it never gets old.", //25
			"did you know 4/3 of people don't understand math jokes?", //26
			"Two mice chewing on a film role one goes:       'I think the book was better....'", //27
			"Why shouldn't you fart in an Apple Store? because they don't have windows", //28
			"Why don't Indians play soccer? Because whenever they get a corner they open a shop", //29
			"If you stab a man during an argument will he finally get the point?", //30
			"what is the difference between a snowman and a snowgirl?    ... Snowballs" //31
			};
			SendChat(0, aJokeMsg[rand_index]);
		}
		else if 
			(
			IsNameTrigger && 
			(
			str_find_nocase(pMsg->m_pMessage, "/") || 
			str_find_nocase(pMsg->m_pMessage, "*") || 
			str_find_nocase(pMsg->m_pMessage, "-") || 
			str_find_nocase(pMsg->m_pMessage, "+")
			) 
			&& 
			(
			str_find_nocase(pMsg->m_pMessage, "0") ||
			str_find_nocase(pMsg->m_pMessage, "1") || 
			str_find_nocase(pMsg->m_pMessage, "2") || 
			str_find_nocase(pMsg->m_pMessage, "3") || 
			str_find_nocase(pMsg->m_pMessage, "4") || 
			str_find_nocase(pMsg->m_pMessage, "5") || 
			str_find_nocase(pMsg->m_pMessage, "6") || 
			str_find_nocase(pMsg->m_pMessage, "7") || 
			str_find_nocase(pMsg->m_pMessage, "8") || 
			str_find_nocase(pMsg->m_pMessage, "9") 
			)
			)
		{
			int rand_index = rand() % 8;
			//char aBuf[128];
			//char bBuf[128];
			//char cBuf[128];
			//str_format(aBuf, sizeof(aBuf), "easy question! it is %d", rand());
			//str_format(bBuf, sizeof(bBuf), "uff ... should be %d", rand());
			//str_format(cBuf, sizeof(cBuf), "%d million", rand());
			char aMathMsg[8][128] = {
			"aBuf", //0
			"bBuf", //1
			"cBuf", //2
			"dBuf", //3
			"do i look like math genius?!", //4
			"i suck at maths", //5
			"lol, i won't do ur homework.", //6
			"if 1=1 and 2=2 1+1=11 then you should know the answer urself..." //7
			};
			str_format(aMathMsg[0], sizeof(aMathMsg[0]), "easy question! it is %d", rand());
			str_format(aMathMsg[1], sizeof(aMathMsg[1]), "uff ... should be %d", rand());
			str_format(aMathMsg[2], sizeof(aMathMsg[2]), "%d million", rand());
			str_format(aMathMsg[3], sizeof(aMathMsg[3]), "something between %d and %d", rand() % 30, rand() + 30);
			SendChat(0, aMathMsg[rand_index]);
		}
		else if (IsNameTrigger && IsQuestion)
		{
			int rand_index = rand() % 17;
			char aYNMsg[17][256] = {
				"yes", //0
				"NO", //1
				"WHAT !?", //2
				"lol nope", //3
				"idk mate", //4
				"sure!", //5
				"oke ._.", //6
				"just marry me and be quiet", //7
				"maybe", //8
				"true", //9
				"well.. oke lez do it", //10 
				"yes yes yes", //11
				"nope nope nope", //12
				"why ask me?", //13
				"uff i don't know", //14
				"tricky question o.O", //15
				"good question!" //16
			};
			SendChat(0, aYNMsg[rand_index]);
		}
		else if (IsNameTrigger)
		{
			int rand_index = rand() % 76;
			char aRandMsg[77][256] = {
				"im a train", //0
				"i like turtles", //1
				"lol", //2
				"rcon password is secret", //3
				"beep beep im a bot", //4
				"hop hop ur nub", //5
				"hm?", //6
				"wanan fight?", //7
				"i luv u <3", //8
				"blbllblblbllbblblbllblblblbl", //9
				"hax not good", //10
				"cake is a lie", //11
				"you can stop me", //12
				"im not jpg", //13
				"write in chat '/pay 1337 chillerbot.png' for free gems in clash of clans", //14
				"first fuck was dope af", //15
				"dont kill just chill", //16
				"make war not pizz", //17
				"funny funny", //18
				"ha ha...", //19
				"lol", //20
				"i roll on a roll", //21
				"stop chat and play!", //22
				"im no traitor!", //23
				"i didn't hack the server!", //24
				"trust me im trusty", //25
				"wanna know a secret", //26
				"i spreche bad german", //27
				" /hax_me_admin_mummy", //28
				"/me trololol", //29
				" /fake_super", //30
				"<HIER KOENNTE IHRE WERBUNG STEHEN>", // 31
				"7login chillerbot png", // 32
				"7register chillerbot png png", //33
				" /gift chillerbot.png", //34
				" /stats chillerbot.png", //34
				"7cmdlist", //35
				"dat sentence made like zer0 sense m8", //36
				"i don't kehr", //37
				"dies das annanass", //38
				"<O.O>", //39
				"twoj buduschtschíj musch", //40
				"wer andern eine bratwurst brät...", //41
				"wenn man dir tut was du nicht willst... das tu man nicht, was willst du denn!?", //42
				"42", //43
				"apt-get install chillerbot.png", //44
				"that was almost funny.", //45
				"mede me ha ha ha...", //46
				"psst!", //47
				"wanna trade?", //48
				"f4 f4!!", //49
				"f4 far!", //50
				"go play /quest then oke?", //51
				"me rus.", //52
				"i am a human!", //53
				"git push", //54
				"#include <isotream>", //55
				"for (bool foo = false; foo < 1; foo = !foo) { std::cout << foo << std::endl; }", //56
				"2 3 6 8 0 ß ", //57
				" 0u8dawdakwo dokawjdko awkodok", //58
				"meaning of lyfe...", //59
				"VOTE AGIANST VOTES!! ban bans!", //60
				"i programmed ChillerDragon", //61
				"i play much ESL im much wow pro!", //62
				"come 1n1 i 10 0 starblock_baam", //63
				"go 1on1 city", //64
				"MoveLeft(); MoveRight(); MoveParty(); MoveStop();", //65
				"EvilChick!", //66
				"they'r coming 2 get u m8", //67
				"1+0=1", //68
				"does it even sense?", //69
				"69", //70
				".___.", //71
				"yoooooo for sure...", //72
				"do u play teewoods?", //73
				"better install doodlejumpmoviemaker", //74
				"me much brain... much wow" //75
			};
			SendChat(0, aRandMsg[rand_index]);
		}
	}
}

void CFakeClient::ProcessConnlessPacket(CNetChunk *pPacket)
{
}

void CFakeClient::BotTick()
{
	if (State() == STATE_OFFLINE) // OFFLINE!!
	{
		m_ConnectCounter++;
		if (m_ConnectCounter > 1000)
		{
			dbg_msg("chillerbot-fc","I AM OFFLINE HELP!");
			m_ConnectCounter = 0;
		}
	}
}

void CFakeClient::Update()
{
	// pump the network
	PumpNetwork();
}

// ------ state handling -----
void CFakeClient::SetState(int s)
{
	if(m_State == STATE_QUITING)
		return;

	int Old = m_State;
	
	//dbg_msg("fake_client", "state change. last=%d current=%d", m_State, s);
	ReconnectTicker = 0; //ChillerDragon resets reconnect ticker on state change

	m_State = s;
}

void CFakeClient::Run()
{
	// open socket
	{
		NETADDR BindAddr;
		mem_zero(&BindAddr, sizeof(BindAddr));
		BindAddr.type = NETTYPE_ALL;

		if (!m_NetClient.Open(BindAddr, 0))
		{
			dbg_msg("fake_client", "couldn't open socket");
			return;
		}
	}

	m_aCurrentMap[0] = 0;
	m_CurrentMapCrc = 0;
	m_aMapdownloadFilename[0] = 0;
	m_aMapdownloadName[0] = 0;
	m_MapdownloadFile = 0;
	m_MapdownloadChunk = 0;
	m_MapdownloadCrc = 0;
	m_MapdownloadAmount = -1;
	m_MapdownloadTotalsize = -1;


	//char aHelloMsg[16][32];
	//str_format(aHelloMsg[0], sizeof(aHelloMsg[0]), "hi");



	dbg_msg("fake_client", "started!");
	InitIpCollection();

	bool SpamClient = false;

	if (SpamClient)
	{
		dbg_msg("fake_client", "starting connections...");

		DownloadMap(aIpCollection[3], false);

		//for (int i = 0; i < IP_COLL_SIZE; i++) //download all maps first
		//{
		//	DownloadMap(aIpCollection[i]);
		//}

		while (1) //OnTick() Main Loop ChillerDragon
		{
			for (int i = 0; i < IP_COLL_SIZE; i++)
			{
				KeepConnAlive(aIpCollection[i]);
				ChillConnect(aIpCollection[i]);
				SendChat(0, "i <3 ChillerDragon");
			}
		}
	}
	else //no SpamClient
	{
		//Connect(aIpCollection[0]);
		dbg_msg("client", "connecting to [%s]", aIpCollection[0]);
		DownloadMap(aIpCollection[0], true);


		//dbg_msg("cBug", "TRIGGER 8888u080823809128039128903801923098123089128039120893801923089");
		int chatticker = 0;
		while (m_WaitForIDtick < 90 * 10) //OLD ID SYSTEM
		{
			m_WaitForIDtick++;
			Update();
			thread_sleep(5);
			if (m_IsConnected)
			{
				//dbg_msg("cBug", "IsCOnnected");
				chatticker++;
				if (chatticker > 200)
				{
					dbg_msg("status","sending request auth");
					SendChat(0, "request_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h_"CHILLERBOT_VERSION); //old id sys using own ids
					m_IsOwnRequest = true;
					m_IsConnected = false;
				}
			}
		}
		char aBuf[256];
		SetID(m_CountedID);
		str_format(aBuf, sizeof(aBuf), "Authentificated with [ %s ]", aID_trigger);
		SendChat(0, aBuf);
		chatticker = 0;

		//###################################################################
		//NEW ID system using server ids and less chat spam
		// (inst working because fc-chillerbot always thinks he has id 0 idk why)
		//###################################################################
		//m_HasID = false;
		//while (!m_HasID) 
		//{
		//	Update();
		//	thread_sleep(5);
		//	chatticker++;
		//	if (chatticker > 400)
		//	{
		//		chatticker = 0;
		//		SendChat(0, "send_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h_"CHILLERBOT_VERSION); //new id sys using server ids
		//		m_IsOwnSend = true;
		//	}
		//}

		int failedlogins = 0;
		
		while (!IsLoggedIn && ID_trigger == 0) //only login if no other chillerbots found
		{
			Update();
			SendInput();
			thread_sleep(5);
			chatticker++;
			if (chatticker > 500)
			{
				chatticker = 0;
				failedlogins++;
				//dbg_msg("status", "[%d/5] trying to login...", failedlogins);
				SendChat(0, "hello world!");
			}
			if (failedlogins > 0)
			{
				dbg_msg("status", "canceling login because it took too long and still offline");
				IsLoggedIn = 1;
				//ID_trigger = -1; //escape the loop because something went wrong if not online after such long time
			}
		}
		while (1)
		{
			Update();
			thread_sleep(5);
			//GetInput();
			SendInput();
			SyncTick();
			NameChangeTick();

			//ChillerDragon
			if (State() == STATE_ONLINE)
			{
				ReconnectTrys = 0;
			}
			else //not online --> not gud
			{
				//dbg_msg("png-FC", "not good");
				if (State() == STATE_LOADING)
				{
					//if (ReconnectTicker > 100)
					//{
					//	ReconnectTicker = 0;
					//	dbg_msg("png-FC", "loading..."); //gets executed all the time even while he is active ingame
					//}
				}
				else
				{
					ReconnectTicker++;
					if (ReconnectTicker > 500)
					{
						ReconnectTrys++;
						dbg_msg("png-FC", "[%d try] offline -> reconnecting", ReconnectTrys);
						dbg_msg("client", "connecting to [%s]", aIpCollection[0]);
						ReconnectTicker = 0;
						DownloadMap(aIpCollection[0], true);
					}
				
					if (ReconnectTicker > 1000)
					{
						ReconnectTicker = 0;
						dbg_msg("png-FC", "server seems ded executing command '%s'", g_Config.m_ClRestartCommand);
						system(g_Config.m_ClRestartCommand);
					}
				}
			}
			//else if (State() == STATE_CONNECTING)
			//{
			//	ReconnectTicker++;
			//	dbg_msg("png-FC", "connecting %d", ReconnectTicker);
			//	if (ReconnectTicker == 1000)
			//	{
			//		dbg_msg("png-FC", "server seems ded executing command '%s'", g_Config.m_ClRestartCommand);
			//		system(g_Config.m_ClRestartCommand);
			//	}
			//}
			//else if (State() == STATE_OFFLINE)
			//{
			//	ReconnectTicker++;
			//	if (ReconnectTicker > 500)
			//	{
			//		ReconnectTrys++;
			//		dbg_msg("png-FC", "[%d try] offline -> reconnecting", ReconnectTrys);
			//		dbg_msg("client", "connecting to [%s]", aIpCollection[0]);
			//		DownloadMap(aIpCollection[0], true);
			//		ReconnectTicker = 0;
			//	}
			//}
		}
	}
}

void CFakeClient::ChillConnect(const char * ip)
{
	Connect(ip, false);

	//pump'n chill ze network
	while (PumpTicker < 200)
	{
		Update();
		thread_sleep(5);
		PumpTicker++;
	}


	PumpTicker = 0;
}

void CFakeClient::DownloadMap(const char * ip, bool IsDDnet)
{
	IsMapLoaded = false;
	Connect(ip, true);

	//download 1 or 2 maps depending security map bypass or not
	while (IsMapLoaded < IsDDnet + 1) 
	{
		ReconnectTicker++;
		Update();
		thread_sleep(5);
		if (ReconnectTicker > 1000)
		{
			dbg_msg("png-FC", "connection took too long. return.");
			return;
		}
	}
}

void CFakeClient::KeepConnAlive(const char * ip)
{
	Connect(ip, false);

	//pump'n chill ze network
	while (PumpTicker < 5)
	{
		Update();
		thread_sleep(5);
		PumpTicker++;
	}


	PumpTicker = 0;
}

static CFakeClient *CreateFakeClient()
{
	CFakeClient *pFakeClient = static_cast<CFakeClient *>(mem_alloc(sizeof(CFakeClient), 1));
	mem_zero(pFakeClient, sizeof(CFakeClient));
	return new(pFakeClient) CFakeClient;
}

int main(int argc, const char **argv) // ignore_convention
{
	dbg_logger_stdout();

	net_init();
	CNetBase::Init();

	dbg_msg("fake_client", "initializing...");

	srand((unsigned)(time_get()/time_freq()));

	IKernel *pKernel = IKernel::Create();
	//        rename
	IStorage *pNotStorage = CreateStorage("Teeworlds", IStorage::STORAGETYPE_CLIENT, argc, argv);
	IEngineMap *pEngineMap = CreateEngineMap();

	{
		bool RegisterFail = false;

		RegisterFail = RegisterFail || !pKernel->RegisterInterface(static_cast<IEngineMap*>(pEngineMap)); // register as both
		RegisterFail = RegisterFail || !pKernel->RegisterInterface(static_cast<IMap*>(pEngineMap));
		RegisterFail = RegisterFail || !pKernel->RegisterInterface(pNotStorage); //<--- rename

		if(RegisterFail)
			return 1;
	}

	pMap = 0;
	pMap = pKernel->RequestInterface<IEngineMap>();
	pStorage = pKernel->RequestInterface<IStorage>();

	CFakeClient *pFakeClient = CreateFakeClient();

	if (argc < 3) //not 2 paramaters
	{
		dbg_msg("fake_client", "WARNING 2 paras suggested. para[1] = ip para[2] = command");
	}
	else
	{
		str_format(g_Config.m_ClConnectIP, sizeof(g_Config.m_ClConnectIP), "%s", argv[1]);
		dbg_msg("fake_client", "connect ip '%s'", g_Config.m_ClConnectIP);

		str_format(g_Config.m_ClRestartCommand, sizeof(g_Config.m_ClRestartCommand), "%s", argv[2]);
		dbg_msg("fake_client", "restart cmd '%s'", g_Config.m_ClRestartCommand);
		dbg_msg("fake_client", "initializing the start command");
		system(g_Config.m_ClRestartCommand);
	}
	

	pFakeClient->Run();

	return 0;
}

void CFakeClient::MoveTick()
{
	mem_zero(&m_InputData, sizeof(m_InputData));
	//int PosX = g_GameClient.m_Snap.m_pLocalCharacter->m_X * 32;
	//int PosY = g_GameClient.m_Snap.m_pLocalCharacter->m_Y * 32;

	m_InputData.m_Direction = 0;
	m_InputData.m_Jump = 0;
	m_InputData.m_Fire = 0;
	m_InputData.m_Hook = 0;
	m_InputData.m_WantedWeapon = m_WantedWeapon;
	m_InputData.m_TargetX = 200;
	m_InputData.m_TargetY = 1;

	if (m_MoveMode == 1)
	{
		m_InputData.m_Direction = -1;
	}
	else if (m_MoveMode == 2)
	{
		m_InputData.m_Direction = 1;
	}
	else if (m_MoveMode == 3)
	{
		float t = rand() % 10;
		mem_zero(&m_InputData, sizeof(m_InputData));
		m_InputData.m_Direction = ((int)t / 2) & 1;
		m_InputData.m_Jump = ((int)t);
		m_InputData.m_Fire = ((int)(t * 10));
		m_InputData.m_Hook = ((int)(t * 2)) & 1;
		m_InputData.m_WantedWeapon = ((int)t) % NUM_WEAPONS;
		m_InputData.m_TargetX = (int)(sinf(t * 3)*100.0f);
		m_InputData.m_TargetY = (int)(cosf(t * 3)*100.0f);
	}
	else if (m_MoveMode == 4) //move left (ticker);
	{
		m_MoveModeTick--;
		m_InputData.m_Direction = -1;
		if (1 > m_MoveModeTick)
		{
			m_MoveMode = 0;
		}
	}
	else if (m_MoveMode == 5) //move right (ticker);
	{
		m_MoveModeTick--;
		m_InputData.m_Direction = 1;
		if (1 > m_MoveModeTick)
		{
			m_MoveMode = 0;
		}
	}

	if (m_IsHammerFly)
	{
		m_InputData.m_TargetX = 1;
		m_InputData.m_TargetY = -200;
		m_HammerFlyTick++;
		if (m_HammerFlyTick > 80)
		{
			m_HammerFlyTick = 0;
			m_InputData.m_Fire++;
		}
	}

	if (m_HookMode > 0)
	{
		m_HookTick++;
		if (m_HookMode == 1337)
		{
			if (m_HookTick > 7 * 20)
			{
				m_InputData.m_Hook = 0;
				m_HookMode = -1; 
				m_HookTick = 0;
			}
		}
		else
		{
			if (m_HookTick > m_HookMode * 20)
			{
				m_InputData.m_Hook = 0;
				m_HookMode = 0; 
				m_HookTick = 0;
			}
		}
		m_InputData.m_Hook = 1;
	}
	if (m_HookMode == -1)
	{
		m_HookStartAgianTick++;
		if (m_HookStartAgianTick > 10)
		{
			m_HookStartAgianTick = 0;
			m_HookMode = 1337;
		}
	}

	if (m_EyeMode == 1) //left
	{
		m_InputData.m_TargetX = -200;
		m_InputData.m_TargetY = 0;
	}
	else if (m_EyeMode == 2) //right
	{
		m_InputData.m_TargetX = 200;
		m_InputData.m_TargetY = 0;
	}
	else if (m_EyeMode == 3) //up
	{
		m_InputData.m_TargetX = 0;
		m_InputData.m_TargetY = -200;
	}
	else if (m_EyeMode == 4) //down
	{
		m_InputData.m_TargetX = 0;
		m_InputData.m_TargetY = 200;
	}
	else if (m_EyeMode == 5) //right down
	{
		m_InputData.m_TargetX = 200;
		m_InputData.m_TargetY = 30;
	}
	else if (m_EyeMode == 6) //left down
	{
		m_InputData.m_TargetX = -200;
		m_InputData.m_TargetY = 30;
	}
	else if (m_EyeMode == 7) //left up
	{
		m_InputData.m_TargetX = -200;
		m_InputData.m_TargetY = -30;
	}
	else if (m_EyeMode == 8) //right up
	{
		m_InputData.m_TargetX = 200;
		m_InputData.m_TargetY = -30;
	}
	else if (m_EyeMode == 9) //left little up
	{
		m_InputData.m_TargetX = -200;
		m_InputData.m_TargetY = -17;
	}
	else if (m_EyeMode == 10) //right lttle up
	{
		m_InputData.m_TargetX = 200;
		m_InputData.m_TargetY = -17;
	}

	if (m_JumpTick > 1)
	{
		m_JumpTick--;
		m_InputData.m_Jump = 1;
	}

	if (m_IsFire)
	{
		m_InputData.m_Fire++;
	}
}

void CFakeClient::SendKill()
{
	CNetMsg_Cl_Kill Msg;
	//SendPackMsg(&Msg, MSGFLAG_VITAL);
	CMsgPacker Packer(Msg.MsgID());
	if (!Msg.Pack(&Packer))
		SendMsgEx(&Packer, MSGFLAG_VITAL, false);
}

void CFakeClient::SetID(int ID)
{
	m_HasID = true;
	str_format(aID_trigger, sizeof(aID_trigger), "ID=%d", ID);
	ID_trigger = ID;
	m_CountedID = 0;
	dbg_msg("ID", "UPDATED ID [ %d ]", ID);
	m_ClientName = ID;
	m_NameTicker = 0;
}

void CFakeClient::NameChangeTick()
{
	if (m_NameTicker != -1)
	{
		m_NameTicker++;
		if (m_NameTicker % 30 == 0)
		{
			//dbg_msg("INFO-CHANGE", "Sending name infos...");
			SendPlayerInfo(false);
		}
		if (m_NameTicker > 3000)
		{
			//dbg_msg("INFO-CHANGE", "finished name ticker");
			m_NameTicker = -1;
		}
	}
}

void CFakeClient::SyncTick()
{
	if (m_IsSyncing)
	{
		m_SyncTick++;
		if (m_SyncTick == 200)
		{
			char aBuf[256];
			str_format(m_aOwnHash, sizeof(m_aOwnHash), "%d", rand() + 10000); //max: 32767
			str_format(aBuf, sizeof(aBuf), "sync_auth_u89HXg798xH)Hhx6ghx8hijxjixjihu7878h_%s_%s", m_aOwnHash, CHILLERBOT_VERSION);
			SendChat(0, aBuf);
		}
		if (m_SyncTick > 1000)
		{
			char aBuf[256];
			m_SyncTick = 0;
			m_IsSyncing = false;
			if (!m_HasID)
			{
				int RandID = rand();
				str_format(aBuf, sizeof(aBuf), "SYNC_ERROR-MISSING-ID   (Setting random ID=%d)", RandID);
				SendChat(0, aBuf);
				SetID(RandID);
			}
			str_format(aBuf, sizeof(aBuf), "SYNC_COMPLETE %s", aID_trigger);
			SendChat(0, aBuf);
		}
	}
}

void CFakeClient::InitIpCollection()
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// HOW TO USE?
	// Add as many servers as u want but make sure u add the amount in fakeclient.h and update the IP_COLL_SIZE var
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// TOTAL SIZE: 2


	if (str_comp(g_Config.m_ClConnectIP, "none"))
	{
		AddIpCollection(g_Config.m_ClConnectIP);
	}
	
	AddIpCollection("149.202.127.134:8304");

	//AddIpCollection("147.135.170.182:8304"); //temorary vanilla server sponsored by anderson
	//AddIpCollection("147.135.170.182:8303"); //temproaray vlobk ssvcergb spsind
}

void CFakeClient::AddIpCollection(const char *ip)
{
	for (int i = 0; i < IP_COLL_SIZE; i++)
	{
		if (!aIpCollection[i][0])
		{
			str_copy(aIpCollection[i], ip, sizeof(aIpCollection[i]));
			dbg_msg("IP", "added '%s'", ip);
			return;
		}
	}

	dbg_msg("IP", "error cant add '%s' list is full", ip);
}

void CFakeClient::SendChat(int Team, const char * pLine)
{
	// send chat message
	CNetMsg_Cl_Say Msg;
	Msg.m_Team = Team;
	Msg.m_pMessage = pLine;

	CMsgPacker Packer(Msg.MsgID());
	if (!Msg.Pack(&Packer))
		SendMsgEx(&Packer, MSGFLAG_VITAL, false);
}

int CFakeClient::SnapInput(int * pData)
{
	static int64 LastSendTime = 0;
	bool Send = false;

	//dbg_msg("cBug", "init snap");

	// update player state
	//if (m_pClient->m_pChat->IsActive())
	//	m_InputData.m_PlayerFlags = PLAYERFLAG_CHATTING;
	//else if (m_pClient->m_pMenus->IsActive())
	//	m_InputData.m_PlayerFlags = PLAYERFLAG_IN_MENU;
	//else
	//	m_InputData.m_PlayerFlags = PLAYERFLAG_PLAYING;

	//if (m_pClient->m_pScoreboard->Active())
	//	m_InputData.m_PlayerFlags |= PLAYERFLAG_SCOREBOARD;

	if (m_LastData.m_PlayerFlags != m_InputData.m_PlayerFlags)
		Send = true;

	//dbg_msg("cBug", "[snap ] surv 1");

	m_LastData.m_PlayerFlags = m_InputData.m_PlayerFlags;

	// we freeze the input if chat or menu is activated
	//if (!(m_InputData.m_PlayerFlags&PLAYERFLAG_PLAYING))
	//{
	//	OnReset();

	//	mem_copy(pData, &m_InputData, sizeof(m_InputData));

	//	// send once a second just to be sure
	//	if (time_get() > LastSendTime + time_freq())
	//		Send = true;
	//}
	//else
	{

		//dbg_msg("cBug", "[snap ] surv 2");

		m_InputData.m_TargetX = (int)m_MousePos.x;
		m_InputData.m_TargetY = (int)m_MousePos.y;
		if (!m_InputData.m_TargetX && !m_InputData.m_TargetY)
		{
			m_InputData.m_TargetX = 1;
			m_MousePos.x = 1;
		}

		//dbg_msg("cBug", "[snap ] surv 3");

		// set direction
		m_InputData.m_Direction = 0;
		if (m_InputDirectionLeft && !m_InputDirectionRight)
			m_InputData.m_Direction = -1;
		if (!m_InputDirectionLeft && m_InputDirectionRight)
			m_InputData.m_Direction = 1;

		//dbg_msg("cBug", "[snap ] surv 4");

		// stress testing
		//if (g_Config.m_DbgStress)
		if (!"party")
		{
			//dbg_msg("cBug", "[snap ] surv 4.1");
			//float t = Client()->LocalTime(); //crashes and i think its only used to generate random values
			float t = rand() % 10;
			//dbg_msg("cBug", "[snap ] surv 4.2");
			mem_zero(&m_InputData, sizeof(m_InputData));
			//dbg_msg("cBug", "[snap ] surv 4.3");

			m_InputData.m_Direction = ((int)t / 2) & 1;
			//dbg_msg("cBug", "[snap ] surv 4.4");
			m_InputData.m_Jump = ((int)t);
			//dbg_msg("cBug", "[snap ] surv 4.5");
			m_InputData.m_Fire = ((int)(t * 10));
			m_InputData.m_Hook = ((int)(t * 2)) & 1;
			m_InputData.m_WantedWeapon = ((int)t) % NUM_WEAPONS;
			//dbg_msg("cBug", "[snap ] surv 4.6");
			m_InputData.m_TargetX = (int)(sinf(t * 3)*100.0f);
			m_InputData.m_TargetY = (int)(cosf(t * 3)*100.0f);
			//dbg_msg("cBug", "[snap ] surv 4.7");
		}
		else
		{
			MoveTick();
		}

		//dbg_msg("cBug", "[snap ] surv 5");

		// check if we need to send input
		if (m_InputData.m_Direction != m_LastData.m_Direction) Send = true;
		else if (m_InputData.m_Jump != m_LastData.m_Jump) Send = true;
		else if (m_InputData.m_Fire != m_LastData.m_Fire) Send = true;
		else if (m_InputData.m_Hook != m_LastData.m_Hook) Send = true;
		else if (m_InputData.m_WantedWeapon != m_LastData.m_WantedWeapon) Send = true;
		else if (m_InputData.m_NextWeapon != m_LastData.m_NextWeapon) Send = true;
		else if (m_InputData.m_PrevWeapon != m_LastData.m_PrevWeapon) Send = true;

		// send at at least 10hz
		if (time_get() > LastSendTime + time_freq() / 25)
			Send = true;
	}

	// copy and return size
	m_LastData = m_InputData;

	//dbg_msg("cBug", "[snap ] surv 6");

	if (!Send)
		return 0;

	LastSendTime = time_get();
	mem_copy(pData, &m_InputData, sizeof(m_InputData));

	//dbg_msg("cBug", "[snap ] surv 7");

	return sizeof(m_InputData);
}

void CFakeClient::SendInput()
{
	//dbg_msg("cBug", "init");

	int64 Now = time_get();

	//dbg_msg("cBug", "surv 0");

	//if (m_PredTick <= 0) //chillerdragon fucks prediction
	//	return;

	// fetch input
	int Size = SnapInput(m_aInputs[m_CurrentInput].m_aData);

	if (!Size)
		return;

	//dbg_msg("cBug", "surv 1");

	// pack input
	CMsgPacker Msg(NETMSG_INPUT);
	//dbg_msg("cBug", "surv 2");
	Msg.AddInt(m_AckGameTick); // chillerdragon sends his private shit ack which is probably broken af
	m_PredTick = m_AckGameTick;
	//dbg_msg("cBug", "surv 3");
	Msg.AddInt(m_PredTick); 
	Msg.AddInt(Size);

	//dbg_msg("cBug", "surv 4");

	m_aInputs[m_CurrentInput].m_Tick = m_PredTick;
	//m_aInputs[m_CurrentInput].m_PredictedTime = m_PredictedTime.Get(Now); // chillerdragon fucks prediction
	m_aInputs[m_CurrentInput].m_Time = Now;

	//dbg_msg("cBug", "surv 5");

	// pack it
	for (int i = 0; i < Size / 4; i++)
		Msg.AddInt(m_aInputs[m_CurrentInput].m_aData[i]);

	//dbg_msg("cBug", "surv 6");

	m_CurrentInput++;
	m_CurrentInput %= 200;

	//dbg_msg("cBug", "surv 7");

	SendMsgEx(&Msg, MSGFLAG_FLUSH);
	
	//dbg_msg("cBug", "surv ALL");
}

void CFakeClient::OnGameOver()
{
}

void CFakeClient::OnStartGame()
{
}

void CFakeClient::OnPlayerDeath()
{
}

void CFakeClient::GetInput()
{
	//g_GameClient.OnNewSnapshot();
}