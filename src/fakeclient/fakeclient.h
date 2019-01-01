#ifndef FAKECLIENT_H
#define FAKECLIENT_H

#include <engine/kernel.h>
#include <base/vmath.h> //ChillerDragon (used for snaps (movement output))
#include <game/layers.h> //ChillerDragon (used for snaps (input like names pos and skins etc))
#include <game/gamecore.h> //ChillerDragon (core sounds important xd)
#include <stdio.h> //ChillerDragon sscanf

#define CHILLERBOT_VERSION "0003"
char aBotVersion[5] = {"0003"};

#define BLMAPCHILL
//#define CHILLBLOCK5

class CFakeClient
{
public:

	void Run();

	int SendMsgEx(CMsgPacker *pMsg, int Flags, bool System=true);
	void Connect(const char *pAddress, bool ClearDC = true);
	void Disconnect(bool ClearDC = true);

	// ------ state handling -----
	void SetState(int s);

	inline int State() const { return m_State; }

	enum
	{
		STATE_OFFLINE=0,
		STATE_CONNECTING,
		STATE_LOADING,
		STATE_ONLINE,
		STATE_DEMOPLAYBACK,
		STATE_QUITING,
	};

	//########################################
	//ChillerDragon ( vanilla tw importations)
	//########################################

	void SendChat(int Team, const char *pLine);
	int SnapInput(int *pData);
	void SendInput();
	void GetInput();
	void OnGameOver();
	void OnStartGame();
	void OnPlayerDeath();

	//gameclient.h



	bool m_NewTick;
	
	//CSnapState m_Snap;
	//class IClient *m_pClient;
	//class IClient *Client() const { return m_pClient; }

	//controls.h
	vec2 m_MousePos;
	vec2 m_TargetPos;

	CNetObj_PlayerInput m_InputData;
	CNetObj_PlayerInput m_LastData;
	int m_InputDirectionLeft;
	int m_InputDirectionRight;

	//client.h
	int m_AckGameTick;
	int m_PredTick; //wtf shouldnt be here and anyways is total chaos (ChillerDragon)

	// input
	struct // TODO: handle input better
	{
		int m_aData[MAX_INPUT_SIZE]; // the input data
		int m_Tick; // the tick that the input is for
		int64 m_PredictedTime; // prediction latency when we sent this input
		int64 m_Time;
	} m_aInputs[200];

	int m_CurrentInput;


	//##################################
	// ChillerDragon (selfmade stuff)
	//##################################

	void SendKill();
	void MoveTick();
	int m_WantedWeapon;
	bool m_IsFire;
	int m_MoveMode;
	int m_MoveModeTick;
	bool m_IsHammerFly;
	int m_HammerFlyTick;
	int m_DoMode;
	int m_HookMode;
	int m_HookTick;
	int m_HookStartAgianTick;
	int m_EyeMode;
	int m_JumpTick;
	int m_WaitForIDtick;
	int m_CountedID;
	void SetID(int ID);
	char aID_trigger[16];
	int ID_trigger;
	bool m_HasID;
	bool m_IsOutdated;
	bool m_IsOwnRequest;
	bool m_IsOwnSend;
	bool m_IsIgnoreChat;
	int m_ClientName;
	void NameChangeTick();
	int m_NameTicker;

	int m_SyncTick;
	bool m_IsSyncing;
	char m_aOwnHash[6];
	void SyncTick();

	bool m_IsConnected;

	int64 m_ConnectCounter;
	int m_CurrentSpamIP;
	bool IsAfkChillerDragon;
	bool IsLoggedIn;

	int ReconnectTicker;
	int ReconnectTrys;

	enum
	{
		IP_COLL_SIZE=34, //ddnet = 22 total = 34
	};

	char aIpCollection[IP_COLL_SIZE][32];

	void InitIpCollection();
	void AddIpCollection(const char * ip);
	void ChillConnect(const char * ip);
	void DownloadMap(const char * ip, bool IsDDnet = true); //uses clear disconnect
	void KeepConnAlive(const char * ip);

	int IsMapLoaded;


	int PumpTicker;

private:

	class CNetClient m_NetClient;
	CNetObjHandler m_NetObjHandler;
	
	void BotTick();
	void Update();
	void PumpNetwork();

	void SendReady();
	void SendInfo();
	void SendEnterGame();
	void EnterGame();
	void SendPlayerInfo(bool start);

	void ProcessServerPacket(CNetChunk *pPacket);
	void ProcessConnlessPacket(CNetChunk *pPacket);
	void OnMessage(int MsgId, CUnpacker *pUnpacker);

	const char *LoadMap(const char *pName, const char *pFilename, unsigned WantedCrc);
	const char *LoadMapSearch(const char *pMapName, int WantedCrc);

	char m_aServerAddressStr[256];
	NETADDR m_ServerAddress;

	int m_State;

	char m_aCurrentMap[256];
	unsigned m_CurrentMapCrc;
	char m_aMapdownloadFilename[256];
	char m_aMapdownloadName[256];
	IOHANDLE m_MapdownloadFile;
	int m_MapdownloadChunk;
	int m_MapdownloadCrc;
	int m_MapdownloadAmount;
	int m_MapdownloadTotalsize;
};



//class CGameClient : public IGameClient
//{
//	class CStack
//	{
//	public:
//		enum
//		{
//			MAX_COMPONENTS = 64,
//		};
//
//		CStack();
//		void Add(class CComponent *pComponent);
//
//		class CComponent *m_paComponents[MAX_COMPONENTS];
//		int m_Num;
//	};
//
//	CStack m_All;
//	CStack m_Input;
//	CNetObjHandler m_NetObjHandler;
//
//	class IEngine *m_pEngine;
//	class IInput *m_pInput;
//	class IGraphics *m_pGraphics;
//	class ITextRender *m_pTextRender;
//	class IClient *m_pClient;
//	class ISound *m_pSound;
//	class IConsole *m_pConsole;
//	class IStorage *m_pStorage;
//	class IDemoPlayer *m_pDemoPlayer;
//	class IDemoRecorder *m_pDemoRecorder;
//	class IServerBrowser *m_pServerBrowser;
//	class IEditor *m_pEditor;
//	class IFriends *m_pFriends;
//
//	CLayers m_Layers;
//	class CCollision m_Collision;
//	//CUI m_UI;
//
//	void DispatchInput();
//	void ProcessEvents();
//	void UpdatePositions();
//
//	int m_PredictedTick;
//	int m_LastNewPredictedTick;
//
//	int64 m_LastSendInfo;
//
//	//static void ConTeam(IConsole::IResult *pResult, void *pUserData);
//	//static void ConKill(IConsole::IResult *pResult, void *pUserData);
//
//	//static void ConchainSpecialInfoupdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);
//
//public:
//	IKernel *Kernel() { return IInterface::Kernel(); }
//	IEngine *Engine() const { return m_pEngine; }
//	class IGraphics *Graphics() const { return m_pGraphics; }
//	class IClient *Client() const { return m_pClient; }
//	//class CUI *UI() { return &m_UI; }
//	class ISound *Sound() const { return m_pSound; }
//	class IInput *Input() const { return m_pInput; }
//	class IStorage *Storage() const { return m_pStorage; }
//	class IConsole *Console() { return m_pConsole; }
//	class ITextRender *TextRender() const { return m_pTextRender; }
//	class IDemoPlayer *DemoPlayer() const { return m_pDemoPlayer; }
//	class IDemoRecorder *DemoRecorder() const { return m_pDemoRecorder; }
//	class IServerBrowser *ServerBrowser() const { return m_pServerBrowser; }
//	//class CRenderTools *RenderTools() { return &m_RenderTools; }
//	class CLayers *Layers() { return &m_Layers; };
//	class CCollision *Collision() { return &m_Collision; };
//	class IEditor *Editor() { return m_pEditor; }
//	class IFriends *Friends() { return m_pFriends; }
//
//	int NetobjNumCorrections() { return m_NetObjHandler.NumObjCorrections(); }
//	const char *NetobjCorrectedOn() { return m_NetObjHandler.CorrectedObjOn(); }
//
//	bool m_SuppressEvents;
//	bool m_NewTick;
//	bool m_NewPredictedTick;
//	int m_FlagDropTick[2];
//
//	// TODO: move this
//	CTuningParams m_Tuning;
//
//	enum
//	{
//		SERVERMODE_PURE = 0,
//		SERVERMODE_MOD,
//		SERVERMODE_PUREMOD,
//	};
//	int m_ServerMode;
//
//	int m_DemoSpecID;
//
//	vec2 m_LocalCharacterPos;
//
//	// predicted players
//	CCharacterCore m_PredictedPrevChar;
//	CCharacterCore m_PredictedChar;
//
//	// snap pointers
//	struct CSnapState
//	{
//		const CNetObj_Character *m_pLocalCharacter;
//		const CNetObj_Character *m_pLocalPrevCharacter;
//		const CNetObj_PlayerInfo *m_pLocalInfo;
//		const CNetObj_SpectatorInfo *m_pSpectatorInfo;
//		const CNetObj_SpectatorInfo *m_pPrevSpectatorInfo;
//		const CNetObj_Flag *m_paFlags[2];
//		const CNetObj_GameInfo *m_pGameInfoObj;
//		const CNetObj_GameData *m_pGameDataObj;
//		int m_GameDataSnapID;
//
//		const CNetObj_PlayerInfo *m_paPlayerInfos[MAX_CLIENTS];
//		const CNetObj_PlayerInfo *m_paInfoByScore[MAX_CLIENTS];
//		const CNetObj_PlayerInfo *m_paInfoByTeam[MAX_CLIENTS];
//
//		int m_LocalClientID;
//		int m_NumPlayers;
//		int m_aTeamSize[2];
//
//		// spectate data
//		struct CSpectateInfo
//		{
//			bool m_Active;
//			int m_SpectatorID;
//			bool m_UsePosition;
//			vec2 m_Position;
//		} m_SpecInfo;
//
//		//
//		struct CCharacterInfo
//		{
//			bool m_Active;
//
//			// snapshots
//			CNetObj_Character m_Prev;
//			CNetObj_Character m_Cur;
//
//			// interpolated position
//			vec2 m_Position;
//		};
//
//		CCharacterInfo m_aCharacters[MAX_CLIENTS];
//	};
//
//	CSnapState m_Snap;
//
//	// client data
//	struct CClientData
//	{
//		int m_UseCustomColor;
//		int m_ColorBody;
//		int m_ColorFeet;
//
//		char m_aName[MAX_NAME_LENGTH];
//		char m_aClan[MAX_CLAN_LENGTH];
//		int m_Country;
//		char m_aSkinName[64];
//		int m_SkinID;
//		int m_SkinColor;
//		int m_Team;
//		int m_Emoticon;
//		int m_EmoticonStart;
//		CCharacterCore m_Predicted;
//
//		//CTeeRenderInfo m_SkinInfo; // this is what the server reports
//		//CTeeRenderInfo m_RenderInfo; // this is what we use
//
//		float m_Angle;
//		bool m_Active;
//		bool m_ChatIgnore;
//		bool m_Friend;
//
//		void UpdateRenderInfo() {};
//		void Reset() {};
//	};
//
//	CClientData m_aClients[MAX_CLIENTS];
//
//	//CRenderTools m_RenderTools;
//
//	void OnReset();
//
//	// hooks
//	virtual void OnConnected();
//	virtual void OnRender();
//	virtual void OnRelease();
//	virtual void OnInit();
//	virtual void OnConsoleInit();
//	virtual void OnStateChange(int NewState, int OldState);
//	virtual void OnMessage(int MsgId, CUnpacker *pUnpacker);
//	virtual void OnNewSnapshot(); //was orginally OnNewSnapshot()
//	virtual void OnPredict();
//	virtual void OnActivateEditor();
//	virtual int OnSnapInput(int *pData);
//	virtual void OnShutdown();
//	virtual void OnEnterGame();
//	virtual void OnRconLine(const char *pLine);
//	virtual void OnGameOver();
//	virtual void OnStartGame();
//
//	virtual const char *GetItemName(int Type);
//	virtual const char *Version();
//	virtual const char *NetVersion();
//
//
//	// actions
//	// TODO: move these
//	void SendSwitchTeam(int Team);
//	void SendInfo(bool Start);
//	void SendKill(int ClientID);
//
//	// pointers to all systems
//	class CGameConsole *m_pGameConsole;
//	class CBinds *m_pBinds;
//	class CParticles *m_pParticles;
//	class CMenus *m_pMenus;
//	class CSkins *m_pSkins;
//	class CCountryFlags *m_pCountryFlags;
//	class CFlow *m_pFlow;
//	class CChat *m_pChat;
//	class CDamageInd *m_pDamageind;
//	class CCamera *m_pCamera;
//	class CControls *m_pControls;
//	class CEffects *m_pEffects;
//	class CSounds *m_pSounds;
//	class CMotd *m_pMotd;
//	class CMapImages *m_pMapimages;
//	class CVoting *m_pVoting;
//	class CScoreboard *m_pScoreboard;
//	class CItems *m_pItems;
//	class CMapLayers *m_pMapLayersBackGround;
//	class CMapLayers *m_pMapLayersForeGround;
//};

//CGameClient *pGameClient() { return new CGameClient; } //testy chillerdragon doin random stuff
//extern CGameClient *pGameClient(); //testy chillerdragon

//class CGameClient : public IGameClient
//{
//	class CStack
//	{
//	public:
//		enum
//		{
//			MAX_COMPONENTS = 64,
//		};
//
//		CStack();
//		void Add(class CComponent *pComponent);
//
//		class CComponent *m_paComponents[MAX_COMPONENTS];
//		int m_Num;
//	};
//
//	CStack m_All;
//	CStack m_Input;
//	CNetObjHandler m_NetObjHandler;
//
//	class IEngine *m_pEngine;
//	class IInput *m_pInput;
//	class IGraphics *m_pGraphics;
//	class ITextRender *m_pTextRender;
//	class IClient *m_pClient;
//	class ISound *m_pSound;
//	class IConsole *m_pConsole;
//	class IStorage *m_pStorage;
//	class IDemoPlayer *m_pDemoPlayer;
//	class IDemoRecorder *m_pDemoRecorder;
//	class IServerBrowser *m_pServerBrowser;
//	class IEditor *m_pEditor;
//	class IFriends *m_pFriends;
//
//	CLayers m_Layers;
//	class CCollision m_Collision;
//	//CUI m_UI; //ChillerDragon removed UI because userinterface is shiet xd
//
//	void DispatchInput();
//	void ProcessEvents();
//	void UpdatePositions();
//
//	int m_PredictedTick;
//	int m_LastNewPredictedTick;
//
//	int64 m_LastSendInfo;
//
//	//static void ConTeam(IConsole::IResult *pResult, void *pUserData); //ChillerDragon 
//	//static void ConKill(IConsole::IResult *pResult, void *pUserData); //ChillerDragon
//
//	//static void ConchainSpecialInfoupdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData); //ChillerDragon
//
//public:
//	IKernel *Kernel() { return IInterface::Kernel(); }
//	IEngine *Engine() const { return m_pEngine; }
//	class IGraphics *Graphics() const { return m_pGraphics; }
//	class IClient *Client() const { return m_pClient; }
//	//class CUI *UI() { return &m_UI; }  //no render no gui client (ChillerDragon)
//	class ISound *Sound() const { return m_pSound; }
//	class IInput *Input() const { return m_pInput; }
//	class IStorage *Storage() const { return m_pStorage; }
//	class IConsole *Console() { return m_pConsole; }
//	class ITextRender *TextRender() const { return m_pTextRender; }
//	class IDemoPlayer *DemoPlayer() const { return m_pDemoPlayer; }
//	class IDemoRecorder *DemoRecorder() const { return m_pDemoRecorder; }
//	class IServerBrowser *ServerBrowser() const { return m_pServerBrowser; }
//	//class CRenderTools *RenderTools() { return &m_RenderTools; }  //no render no gui client (ChillerDragon)
//	class CLayers *Layers() { return &m_Layers; };
//	class CCollision *Collision() { return &m_Collision; };
//	class IEditor *Editor() { return m_pEditor; }
//	class IFriends *Friends() { return m_pFriends; }
//
//	int NetobjNumCorrections() { return m_NetObjHandler.NumObjCorrections(); }
//	const char *NetobjCorrectedOn() { return m_NetObjHandler.CorrectedObjOn(); }
//
//	bool m_SuppressEvents;
//	bool m_NewTick;
//	bool m_NewPredictedTick;
//	int m_FlagDropTick[2];
//
//	// TODO: move this
//	CTuningParams m_Tuning;
//
//	enum
//	{
//		SERVERMODE_PURE = 0,
//		SERVERMODE_MOD,
//		SERVERMODE_PUREMOD,
//	};
//	int m_ServerMode;
//
//	int m_DemoSpecID;
//
//	vec2 m_LocalCharacterPos;
//
//	// predicted players
//	CCharacterCore m_PredictedPrevChar;
//	CCharacterCore m_PredictedChar;
//
//	// snap pointers
//	struct CSnapState
//	{
//		const CNetObj_Character *m_pLocalCharacter;
//		const CNetObj_Character *m_pLocalPrevCharacter;
//		const CNetObj_PlayerInfo *m_pLocalInfo;
//		const CNetObj_SpectatorInfo *m_pSpectatorInfo;
//		const CNetObj_SpectatorInfo *m_pPrevSpectatorInfo;
//		const CNetObj_Flag *m_paFlags[2];
//		const CNetObj_GameInfo *m_pGameInfoObj;
//		const CNetObj_GameData *m_pGameDataObj;
//		int m_GameDataSnapID;
//
//		const CNetObj_PlayerInfo *m_paPlayerInfos[MAX_CLIENTS];
//		const CNetObj_PlayerInfo *m_paInfoByScore[MAX_CLIENTS];
//		const CNetObj_PlayerInfo *m_paInfoByTeam[MAX_CLIENTS];
//
//		int m_LocalClientID;
//		int m_NumPlayers;
//		int m_aTeamSize[2];
//
//		// spectate data
//		struct CSpectateInfo
//		{
//			bool m_Active;
//			int m_SpectatorID;
//			bool m_UsePosition;
//			vec2 m_Position;
//		} m_SpecInfo;
//
//		//
//		struct CCharacterInfo
//		{
//			bool m_Active;
//
//			// snapshots
//			CNetObj_Character m_Prev;
//			CNetObj_Character m_Cur;
//
//			// interpolated position
//			vec2 m_Position;
//		};
//
//		CCharacterInfo m_aCharacters[MAX_CLIENTS];
//	};
//
//	CSnapState m_Snap;
//
//	// client data
//	struct CClientData
//	{
//		int m_UseCustomColor;
//		int m_ColorBody;
//		int m_ColorFeet;
//
//		char m_aName[MAX_NAME_LENGTH];
//		char m_aClan[MAX_CLAN_LENGTH];
//		int m_Country;
//		char m_aSkinName[64];
//		int m_SkinID;
//		int m_SkinColor;
//		int m_Team;
//		int m_Emoticon;
//		int m_EmoticonStart;
//		CCharacterCore m_Predicted;
//
//		//CTeeRenderInfo m_SkinInfo; // this is what the server reports //no render no gui client (ChillerDragon)
//		//CTeeRenderInfo m_RenderInfo; // this is what we use //no render no gui client (ChillerDragon)
//
//		float m_Angle;
//		bool m_Active;
//		bool m_ChatIgnore;
//		bool m_Friend;
//
//		void UpdateRenderInfo();
//		void Reset();
//	};
//
//	CClientData m_aClients[MAX_CLIENTS];
//
//	//CRenderTools m_RenderTools;  //no render no gui client (ChillerDragon)
//
//	void OnReset();
//
//	// hooks
//	virtual void OnConnected();
//	virtual void OnRender();
//	virtual void OnRelease();
//	virtual void OnInit();
//	virtual void OnConsoleInit();
//	virtual void OnStateChange(int NewState, int OldState);
//	virtual void OnMessage(int MsgId, CUnpacker *pUnpacker);
//	virtual void OnNewSnapShot(); //was OnNewSnapshot()
//	virtual void OnPredict();
//	virtual void OnActivateEditor();
//	virtual int OnSnapInput(int *pData);
//	virtual void OnShutdown();
//	virtual void OnEnterGame();
//	virtual void OnRconLine(const char *pLine);
//	virtual void OnGameOver();
//	virtual void OnStartGame();
//
//	virtual const char *GetItemName(int Type);
//	virtual const char *Version();
//	virtual const char *NetVersion();
//
//
//	// actions
//	// TODO: move these
//	void SendSwitchTeam(int Team);
//	void SendInfo(bool Start);
//	void SendKill(int ClientID);
//
//	// pointers to all systems
//	class CGameConsole *m_pGameConsole;
//	class CBinds *m_pBinds;
//	class CParticles *m_pParticles;
//	class CMenus *m_pMenus;
//	class CSkins *m_pSkins;
//	class CCountryFlags *m_pCountryFlags;
//	class CFlow *m_pFlow;
//	class CChat *m_pChat;
//	class CDamageInd *m_pDamageind;
//	class CCamera *m_pCamera;
//	class CControls *m_pControls;
//	class CEffects *m_pEffects;
//	class CSounds *m_pSounds;
//	class CMotd *m_pMotd;
//	class CMapImages *m_pMapimages;
//	class CVoting *m_pVoting;
//	class CScoreboard *m_pScoreboard;
//	class CItems *m_pItems;
//	class CMapLayers *m_pMapLayersBackGround;
//	class CMapLayers *m_pMapLayersForeGround;
//};
//
//virtual void OnNewSnapShot();
//
//// snap pointers
//struct CSnapState
//{
//	const CNetObj_Character *m_pLocalCharacter;
//	const CNetObj_Character *m_pLocalPrevCharacter;
//	const CNetObj_PlayerInfo *m_pLocalInfo;
//	const CNetObj_SpectatorInfo *m_pSpectatorInfo;
//	const CNetObj_SpectatorInfo *m_pPrevSpectatorInfo;
//	const CNetObj_Flag *m_paFlags[2];
//	const CNetObj_GameInfo *m_pGameInfoObj;
//	const CNetObj_GameData *m_pGameDataObj;
//	int m_GameDataSnapID;
//
//	const CNetObj_PlayerInfo *m_paPlayerInfos[MAX_CLIENTS];
//	const CNetObj_PlayerInfo *m_paInfoByScore[MAX_CLIENTS];
//	const CNetObj_PlayerInfo *m_paInfoByTeam[MAX_CLIENTS];
//
//	int m_LocalClientID;
//	int m_NumPlayers;
//	int m_aTeamSize[2];
//
//	// spectate data
//	struct CSpectateInfo
//	{
//		bool m_Active;
//		int m_SpectatorID;
//		bool m_UsePosition;
//		vec2 m_Position;
//	} m_SpecInfo;
//
//	//
//	struct CCharacterInfo
//	{
//		bool m_Active;
//
//		// snapshots
//		CNetObj_Character m_Prev;
//		CNetObj_Character m_Cur;
//
//		// interpolated position
//		vec2 m_Position;
//	};
//
//	CCharacterInfo m_aCharacters[MAX_CLIENTS];
//};



#endif
