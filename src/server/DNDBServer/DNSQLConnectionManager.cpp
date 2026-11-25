#include "stdafx.h"
#include "DNSQLConnectionManager.h"
#include "Log.h"
#include "DNExtManager.h"
#include <iostream>

extern TDBConfig g_Config;

CDNSQLConnectionManager g_SQLConnectionManager;
CDNSQLConnectionManager::CDNSQLConnectionManager(void)
{
	Clear();
}

CDNSQLConnectionManager::~CDNSQLConnectionManager(void)
{
	DestroyDB();
}

void CDNSQLConnectionManager::Clear()
{
	memset(&m_pMembershipArray, 0, sizeof(m_pMembershipArray));
	memset(&m_pWorldArray, 0, sizeof(m_pWorldArray));
}

bool CDNSQLConnectionManager::CreateMembership()
{
	CDNSQLMembership *pDB = NULL;
	for (int i = 0; i < g_Config.nThreadMax; i++){
		pDB = new CDNSQLMembership;

		m_pMembershipArray[i] = pDB;
		m_pMembershipArray[i]->m_cThreadID = i;

		for (int nTryCnt = 0; nTryCnt < 5; nTryCnt++)
		{
			if (pDB->Connect(g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID) < 0)
			{
				g_Log.Log(LogType::_FILELOG, L"Not Connect MembershipDB Ip:%S Port:%d DBName:%s DBID:%s \r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID);
				Sleep(500);
				continue;
			}
			break;
		}

		if( i==0 )
		{
			TDatabaseVersion Version;
			int iRet = pDB->QueryGetDatabaseVersion( &Version );
			if( iRet == ERROR_NONE )
			{
				wprintf(L"DBName:%s Version:%d\n", Version.wszName, Version.iVersion);
				fflush(stdout);
				g_Log.Log(LogType::_FILELOG, L"QueryGetDatabaseVersion Success!! Ip:%S Port:%d DBName:%s DBID:%s Version:%s,%d\r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID, Version.wszName, Version.iVersion );
			}
			else
			{
				g_Log.Log(LogType::_FILELOG, L"QueryGetDatabaseVersion Failed!! Ip:%S Port:%d DBName:%s DBID:%s \r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID);
				return false;
			}
		}

		Sleep(1);
	}

	return true;	
}

bool CDNSQLConnectionManager::CreateWorld()
{
	int nWorldSetID = 0;
	CDNSQLWorld *pDB = NULL;
	for (int j = 0; j < g_Config.nWorldDBCount; j++){
		for (int i = 0; i < g_Config.nThreadMax; i++){
			nWorldSetID = g_Config.WorldDB[j].nWorldSetID;
			pDB = new CDNSQLWorld;

			m_pWorldArray[i][nWorldSetID - 1] = pDB;
			m_pWorldArray[i][nWorldSetID - 1]->m_cThreadID = i;
			m_pWorldArray[i][nWorldSetID - 1]->m_nWorldSetID = nWorldSetID;

			for (int nTryCnt = 0; nTryCnt < 5; nTryCnt++)
			{
				if (pDB->Connect(g_Config.WorldDB[j].szIP, g_Config.WorldDB[j].nPort, g_Config.WorldDB[j].wszDBName, g_Config.WorldDB[j].wszDBID) < 0)
				{
					g_Log.Log(LogType::_FILELOG, L"Not Connect WorldDB Ip:%S Port:%d DBName:%s, DBID:%s \r\n", g_Config.WorldDB[j].szIP, g_Config.WorldDB[j].nPort, g_Config.WorldDB[j].wszDBName, g_Config.WorldDB[j].wszDBID);
					Sleep(500);
					continue;
				}
				break;
			}

			if( i==0 )
			{
				TDatabaseVersion Version;
				int iRet = pDB->QueryGetDatabaseVersion( &Version );
				if( iRet == ERROR_NONE )
				{
					wprintf(L"WorldID[%d] DBName:%s Version:%d\n", j, Version.wszName, Version.iVersion);
					fflush(stdout);
					g_Log.Log(LogType::_FILELOG, L"QueryGetDatabaseVersion Success!! WorldDB Ip:%S Port:%d DBName:%s, DBID:%s Version:%s,%d\r\n", g_Config.WorldDB[j].szIP, g_Config.WorldDB[j].nPort, g_Config.WorldDB[j].wszDBName, g_Config.WorldDB[j].wszDBID, Version.wszName, Version.iVersion );
				}
				else
				{
					g_Log.Log(LogType::_FILELOG, L"QueryGetDatabaseVersion Failed!! WorldDB Ip:%S Port:%d DBName:%s, DBID:%s \r\n", g_Config.WorldDB[j].szIP, g_Config.WorldDB[j].nPort, g_Config.WorldDB[j].wszDBName, g_Config.WorldDB[j].wszDBID);
					return false;
				}
			}

			// g_Log.Log(_SUCCESS, L">> [T:%d, WorldID:%d] Connected to SQLSERVER (%s:%s)\r\n", i, j + 1, g_Config.World[j].wszDSN, g_Config.World[j].wszDBName);

			Sleep(1);
		}
	}

	return true;
}

bool CDNSQLConnectionManager::CreateDB()
{
	if (!CreateMembership()) return false;
	if (!CreateWorld()) return false;

	// Check DB if any cash list item is discontinued
	g_pExtManager->SetProhibitSaleList();

	return true;
}

void CDNSQLConnectionManager::DestroyDB()
{
	int i, j;

	for (i = 0; i < g_Config.nThreadMax; i++){
		SAFE_DELETE(m_pMembershipArray[i]);
	}

	for (i = 0; i < WORLDCOUNTMAX; i++){
		for (j = 0; j < g_Config.nThreadMax; j++){
			SAFE_DELETE(m_pWorldArray[j][i]);
		}
	}

	Clear();
}

CDNSQLMembership* CDNSQLConnectionManager::FindMembershipDB(int nThreadIndex)
{
	if ((nThreadIndex < 0) || (nThreadIndex >= g_Config.nThreadMax)) return NULL;
	return m_pMembershipArray[nThreadIndex];
}

CDNSQLWorld* CDNSQLConnectionManager::FindWorldDB(int nThreadIndex, char cWorldSetID)
{
	if ((cWorldSetID <= 0) || (cWorldSetID > WORLDCOUNTMAX)) return NULL;
	if ((nThreadIndex < 0) || (nThreadIndex >= g_Config.nThreadMax)) return NULL;

	return m_pWorldArray[nThreadIndex][cWorldSetID - 1];
}

CDNSQLWorld* CDNSQLConnectionManager::FindWorldDB(int nThreadIndex)
{
	if ((nThreadIndex < 0) || (nThreadIndex >= g_Config.nThreadMax)) return NULL;
	return m_pWorldArray[nThreadIndex][0];
}
