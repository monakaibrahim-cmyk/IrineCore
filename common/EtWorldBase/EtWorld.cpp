#include "stdafx.h"
#include "EtWorld.h"
#include "EtWorldGrid.h"
#include "EtWorldProp.h"
#include "EtWorldSector.h"
#include "EtTrigger.h"
#include "EtTriggerObject.h"

int CEtWorld::s_nWorldVersion = 100;
char *CEtWorld::s_szWorldHeaderString = "Eternity World Infomation File";
bool CEtWorld::s_bIgnoreInvalidFile = false;

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CEtWorld::CEtWorld()
: m_vPosition( 0.f, 0.f, 0.f ),
  m_vDirection( 0.f, 0.f, 0.f ),
  m_vPrevPosition( 0.f, 0.f, 0.f )
{
	m_pGrid = NULL;
	m_pCurrentGrid = NULL;
	m_fDirectionLength = 0.f;
}

CEtWorld::~CEtWorld()
{
	Destroy();
}

bool CEtWorld::Initialize( const char *szWorldFolder, const char *szGridName )
{
	char szStr[512] = {0, };
	InfoFile_Header Header;
	m_szWorldFolder = szWorldFolder;
	sprintf_s( szStr, "%s\\Infomation", szWorldFolder );

	CResMngStream Stream( szStr );
	if( !Stream.IsValid() ) return false;

	_ASSERT( sizeof(InfoFile_Header) == Stream.Size() );

	Stream.Read( &Header, sizeof(InfoFile_Header) );

	if( strcmp( Header.szStr, s_szWorldHeaderString ) != NULL ) return false;
	if( Header.nVersion != s_nWorldVersion ) return false;

	if( m_pGrid != NULL ) {
		Destroy();
	}
	m_pGrid = AllocGrid();
	if( szGridName == NULL ) {
		if( m_pGrid->Initialize( "Root", 0, 0, 0, 0, 0 ) == false ) {
			SAFE_DELETE( m_pGrid );
			return false;
		}
	}
	else {
		sprintf_s( szStr, "%s\\Grid\\%s\\GridInfo.ini", szWorldFolder, szGridName );
		m_pGrid->SetName( szGridName );
		if( m_pGrid->LoadGrid( szStr ) == false ) {
			SAFE_DELETE( m_pGrid );
			return false;
		}
		m_pCurrentGrid = m_pGrid;
		m_szCurrentGrid = szGridName;
	}
	return true;
}
 
bool CEtWorld::ForceInitialize( int nLoadSectorEnum )
{
	for( DWORD i=0; i<m_pCurrentGrid->GetGridY(); i++ ) {
		for( DWORD j=0; j<m_pCurrentGrid->GetGridX(); j++ ) {
			if( m_pCurrentGrid->LoadSector( SectorIndex( j, i ), false, nLoadSectorEnum ) == false ) return false;
		}
	}
	return true;
}

CEtWorldGrid *CEtWorld::AllocGrid()
{
	return new CEtWorldGrid( this );
}

void CEtWorld::Destroy()
{
	m_pCurrentGrid = NULL;		// Added stability routine.
	SAFE_DELETE( m_pGrid );
}

float CEtWorld::GetHeight( float fX, float fZ, EtVector3 *pNormal, int nTileScale )
{
	if( !m_pCurrentGrid || !m_pGrid ) return 0.f;
	return m_pCurrentGrid->GetHeight( fX, fZ, pNormal, nTileScale );
}

float CEtWorld::GetHeight( EtVector3 &vPos, EtVector3 *pNormal, int nTileScale )
{
	if( !m_pCurrentGrid || !m_pGrid ) return 0.f;
	return m_pCurrentGrid->GetHeight( vPos.x, vPos.z, pNormal, nTileScale );
}

float CEtWorld::GetCollisionHeight( float fX, float fZ, EtVector3 *pNormal, int nTileScale )
{
	if( !m_pCurrentGrid || !m_pGrid ) return 0.f;
	return m_pCurrentGrid->GetCollisionHeight( fX, fZ, pNormal, nTileScale );
}

float CEtWorld::GetCollisionHeight( EtVector3 &vPos, EtVector3 *pNormal, int nTileScale )
{
	if( !m_pCurrentGrid || !m_pGrid ) return 0.f;
	return m_pCurrentGrid->GetCollisionHeight( vPos.x, vPos.z, pNormal, nTileScale );
}


DWORD CEtWorld::GetCurrentTileSize()
{
	if( !m_pCurrentGrid || !m_pGrid  ) return 0;
	return m_pCurrentGrid->GetTileSize();
}

char CEtWorld::GetAttribute( float fX, float fZ )
{
	if( !m_pCurrentGrid || !m_pGrid  ) return 0;
	return m_pCurrentGrid->GetAttribute( fX, fZ );
}

char CEtWorld::GetAttribute( EtVector3 &vPos )
{
	if( !m_pCurrentGrid || !m_pGrid ) return 0;
	return m_pCurrentGrid->GetAttribute( vPos.x, vPos.z );
}

NavigationMesh *CEtWorld::GetNavMesh( EtVector3 &vPos )
{
	if( !m_pCurrentGrid || !m_pGrid ) return 0;
	return m_pCurrentGrid->GetNavMesh( vPos );
}

bool CEtWorld::GetWaterHeight( float fX, float fZ, float *pfHeight )
{
	if( !m_pCurrentGrid || !m_pGrid ) return false;
	return m_pCurrentGrid->GetWaterHeight( fX, fZ, pfHeight );
}

bool CEtWorld::GetWaterHeight( EtVector3 &vPos, float *pfHeight )
{
	if( !m_pCurrentGrid || !m_pGrid ) return false;
	return m_pCurrentGrid->GetWaterHeight( vPos.x, vPos.z, pfHeight );
}

void CEtWorld::CalcWorldBasePos( float fX, float fZ, float &fResultX, float &fResultZ )
{
	if( !m_pCurrentGrid || !m_pGrid ) return;
	m_pCurrentGrid->CalcWorldBasePos( fX, fZ, fResultX, fResultZ );
}

int CEtWorld::GetAttributeBlockSize( float fX, float fZ )
{
	if( !m_pCurrentGrid || !m_pGrid ) return 0;
	return m_pCurrentGrid->GetAttributeBlockSize( fX, fZ );
}

bool CEtWorld::Pick( EtVector3 &vOrig, EtVector3 &vDir, EtVector3 &vPickPos )
{
	if( !m_pCurrentGrid || !m_pGrid ) return false;
	return m_pCurrentGrid->Pick( vOrig, vDir, vPickPos );
}

bool CEtWorld::PickWater( EtVector3 &vOrig, EtVector3 &vDir, EtVector3 &vPickPos )
{
	if( !m_pCurrentGrid || !m_pGrid ) return false;
	return m_pCurrentGrid->PickWater( vOrig, vDir, vPickPos );
}

void CEtWorld::Update( EtVector3 &vPos, EtVector3 &vDir, float fDirLength )
{
	m_vPrevPosition = m_vPosition;
	m_vPosition = vPos;
	m_vDirection = vDir;
	m_fDirectionLength = fDirLength;

}

void CEtWorld::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_pCurrentGrid == NULL || m_pGrid == NULL ) return;

	m_pCurrentGrid->Process( LocalTime, fDelta );
}

CEtWorldGrid *CEtWorld::FindGrid( const char *szGridName, CEtWorldGrid *pGrid )
{
	if( pGrid == NULL ) pGrid = m_pGrid;
	if( pGrid == NULL ) return NULL;
	if( strcmp( szGridName, pGrid->GetName() ) == NULL ) return pGrid;
	for( DWORD i=0; i<pGrid->GetChildCount(); i++ ) {
		CEtWorldGrid *pResult = FindGrid( szGridName, pGrid->GetChildFromIndex(i) );
		if( pResult ) return pResult;
	}
	return NULL;
}

DWORD CEtWorld::GetGridX() 
{ 
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return 0;
	return m_pCurrentGrid->GetGridX();
}

DWORD CEtWorld::GetGridY() 
{ 
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return 0;
	return m_pCurrentGrid->GetGridY();
}

DWORD CEtWorld::GetGridWidth() 
{ 
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return 0;
	return m_pCurrentGrid->GetGridWidth();
}

DWORD CEtWorld::GetGridHeight()
{ 
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return 0;
	return m_pCurrentGrid->GetGridHeight();
}

DWORD CEtWorld::GetGridTileSize()
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return 0;
	return m_pCurrentGrid->GetTileSize();
}

CEtWorldProp *CEtWorld::AddProp( const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale, void *pCustomParam )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return NULL;
	return m_pCurrentGrid->AddProp( szPropName, vPos, vRotate, vScale, pCustomParam );
}

void CEtWorld::ScanProp( EtVector3 &vPos, float fRadius, DNVector(CEtWorldProp*) *pVecResult )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return;
	m_pCurrentGrid->ScanProp( vPos, fRadius, pVecResult );
}

void CEtWorld::ScanDecal( EtVector2 &vPos, float fRadius, std::vector<CEtWorldDecal *> *pVecResult )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return;
	m_pCurrentGrid->ScanDecal( vPos, fRadius, pVecResult );
}

void CEtWorld::ScanEventArea( EtVector2 &vPos, float fRadius, std::vector<CEtWorldEventArea *> *pVecResult )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return;
	m_pCurrentGrid->ScanEventArea( vPos, fRadius, pVecResult );
}

void CEtWorld::ScanEventArea( SAABox &Box, std::vector<CEtWorldEventArea *> *pVecResult )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return;
	m_pCurrentGrid->ScanEventArea( Box, pVecResult );
}

void CEtWorld::ScanEventArea( SOBB &Box, std::vector<CEtWorldEventArea *> *pVecResult )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return;
	m_pCurrentGrid->ScanEventArea( Box, pVecResult );
}

CEtWorldSector *CEtWorld::GetSector( float fX, float fZ )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return NULL;
	return m_pCurrentGrid->GetSector( fX, fZ );
}

void CEtWorld::OnTriggerEventCallback( const char *szFuncName, LOCAL_TIME LocalTime, float fDelta, bool bProcessQueue )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return;
	for( DWORD i=0; i<GetGrid()->GetActiveSectorCount(); i++ ) {
		if( !GetGrid()->GetActiveSector(i)->GetTrigger() ) continue;
		GetGrid()->GetActiveSector(i)->GetTrigger()->OnEventCallback( szFuncName, LocalTime, fDelta, bProcessQueue );
	}
}

void CEtWorld::InsertTriggerEventStore( const char *szValueStr, int nValue )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return;
	for( DWORD i=0; i<GetGrid()->GetActiveSectorCount(); i++ ) {
		if( !GetGrid()->GetActiveSector(i)->GetTrigger() ) continue;
		GetGrid()->GetActiveSector(i)->GetTrigger()->InsertTriggerEventStore( szValueStr, nValue );
	}
}

void CEtWorld::EnableTriggerEventCallback( bool bEnable )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return;
	for( DWORD i=0; i<GetGrid()->GetActiveSectorCount(); i++ ) {
		if( !GetGrid()->GetActiveSector(i)->GetTrigger() ) continue;
		GetGrid()->GetActiveSector(i)->GetTrigger()->EnableEventCallback( bEnable );
	}
}

void CEtWorld::EnableTriggerObject( const char *szTriggerName, bool bEnable )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return;
	for( DWORD i=0; i<GetGrid()->GetActiveSectorCount(); i++ ) {
		if( !GetGrid()->GetActiveSector(i)->GetTrigger() ) continue;
		CEtTriggerObject *pObject = GetGrid()->GetActiveSector(i)->GetTrigger()->GetTriggerFromName( szTriggerName );
		if( pObject ) pObject->SetEnable( bEnable );
	}
}

void CEtWorld::FindPropFromCreateUniqueID( DWORD dwUniqueID, std::vector<CEtWorldProp *> *pVecResult )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return;
	m_pCurrentGrid->FindPropFromCreateUniqueID( dwUniqueID, pVecResult );
}

int CEtWorld::FindEventControlFromUniqueID( int nUniqueID, std::vector<CEtWorldEventControl *> *pVecList )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return 0;
	return m_pCurrentGrid->FindEventControlFromUniqueID( nUniqueID, pVecList );
}

int CEtWorld::FindEventAreaFromName( int nUniqueID, const char *szAreaName, std::vector<CEtWorldEventArea *> *pVecList )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return 0;
	return m_pCurrentGrid->FindEventAreaFromName( nUniqueID, szAreaName, pVecList );
}

int CEtWorld::FindEventAreaFromSubStr( int nUniqueID, const char *szStr, std::vector<CEtWorldEventArea *> *pVecList )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return 0;
	return m_pCurrentGrid->FindEventAreaFromSubStr( nUniqueID, szStr, pVecList );
}

int CEtWorld::FindEventAreaFromCreateUniqueID( int nCreateUniqueID, std::vector<CEtWorldEventArea *> *pVecList )
{
	if( m_pGrid == NULL || m_pCurrentGrid == NULL ) return 0;
	return m_pCurrentGrid->FindEventAreaFromCreateUniqueID( nCreateUniqueID, pVecList );
}

void CEtWorld::CalcWorldSize( float &fCenter, float &fSize )
{
	float fMaxSize = 0.f;
	float fWidthSize = (float)( max( GetGridX() * GetGridWidth() * 100.f,  GetGridY() *  GetGridHeight() * 100.f ) );
	float fTempCenter = 0.f;
	CEtWorldSector *pSector;
	for( DWORD j=0; j<GetGridY(); j++ ) {
		for( DWORD i=0; i<GetGridX(); i++ ) {
			pSector = GetSector( i, j );
			float fTemp = pSector->GetRadius();
			if( fTemp > fMaxSize ) {
				fMaxSize = fTemp;
				fTempCenter = pSector->GetHeightCenter();
			}
		}
	}
	fSize = max( fMaxSize, fWidthSize / 2.f );
	fCenter = fTempCenter;
}


void CEtWorld::CalcWorldSize( EtVector2 &vSize )
{
	vSize.x = (float)( GetGridX() * GetGridWidth() * 100.f );
	vSize.y = (float)( GetGridY() *  GetGridHeight() * 100.f );
}
