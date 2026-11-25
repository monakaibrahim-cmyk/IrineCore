

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

//#include "MEMORYSHUFFLEDATATYPE.H" // P.S.> Used on compilers that support the 'export' keyword
#include <windows.h>
#include <basetsd.h>
#include <new>			// placement new


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define DF_MSDT_LOCKSIGN		(0x6A035984)	// Sync check constant (P.S.> must never be set to 0)	
#define FIX_VIOLATION_BY_FREE // Fix memory corruption caused by freeing an object created with new using free().

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }

///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDervUnit
//*---------------------------------------------------------------
// DESC : Memory shuffle basic unit class (derived)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDervUnit : public CMemoryShuffleBaseUnit<TDATA>
{

private:
	mutable char m_Data[sizeof(TDATA)];

public:
	CMemoryShuffleDervUnit();
	virtual ~CMemoryShuffleDervUnit();

	TDATA GetValue() const;
	void SetValue(TDATA pData);

private:
	void GetXor(char const * pIn, char* pOut, unsigned int pSize, unsigned int pKey) const;
	
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleBaseUnit::CMemoryShuffleBaseUnit
//*---------------------------------------------------------------
// DESC : Constructor
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleBaseUnit<TDATA>::CMemoryShuffleBaseUnit() : m_LastTick(0)
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleBaseUnit::~CMemoryShuffleBaseUnit
//*---------------------------------------------------------------
// DESC : Destructor
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleBaseUnit<TDATA>::~CMemoryShuffleBaseUnit()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleBaseUnit::GetLastTick
//*---------------------------------------------------------------
// DESC : Return timestamp when dynamic memory was last shuffled
// PARM : N/A
// RETV : previous shuffling point
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline unsigned long CMemoryShuffleBaseUnit<TDATA>::GetLastTick() const
{
	return m_LastTick;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleBaseUnit::SetLastTick
//*---------------------------------------------------------------
// DESC : Set timestamp when dynamic memory was last shuffled
// PARM : 1 . pLastTick - New shuffling timestamp
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleBaseUnit<TDATA>::SetLastTick(unsigned long pLastTick)
{
	m_LastTick = pLastTick;
}


//================================================================

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDervUnit::CMemoryShuffleDervUnit
//*---------------------------------------------------------------
// DESC : Constructor
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDervUnit<TDATA>::CMemoryShuffleDervUnit()
{
	::memset(m_Data, 0, sizeof(m_Data));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDervUnit::~CMemoryShuffleDervUnit
//*---------------------------------------------------------------
// DESC : Destructor
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDervUnit<TDATA>::~CMemoryShuffleDervUnit()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDervUnit::GetValue
//*---------------------------------------------------------------
// DESC : Return currently held value
// PARM : N/A
// RETV : previous held value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDervUnit<TDATA>::GetValue() const
{
	TDATA aData;

	GetXor(m_Data, reinterpret_cast<char*>(&aData), sizeof(TDATA), PtrToUint(this));

	return aData;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDervUnit::SetValue
//*---------------------------------------------------------------
// DESC : Set new assigned value
// PARM : 1 . pData - New value to assign
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleDervUnit<TDATA>::SetValue(TDATA pData)
{
	GetXor(reinterpret_cast<char*>(&pData), m_Data, sizeof(TDATA), PtrToUint(this));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDervUnit::GetXor
//*---------------------------------------------------------------
// DESC : XOR a specific value using a conversion key
// PARM : 1 . pIn - Variable holding specific value to convert
//			2 . pOut - variable to output conversion result
//			2 . pSize - size of specific value
//			3 . pKey - Conversion key
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleDervUnit<TDATA>::GetXor(char const * pIn, char* pOut, unsigned int pSize, unsigned int pKey) const
{
	// P.S.> If needed later, pKey could be transformed or the XOR method changed

	unsigned int const * aIn = reinterpret_cast<unsigned int const *>(pIn);	// ++ operator requires an l-value, so temporary variable cannot be completely eliminated (C2105)
	unsigned int* aOut = reinterpret_cast<unsigned int*>(pOut);				// ++ operator requires an l-value, so temporary variable cannot be completely eliminated (C2105)
	unsigned int aQtSize = pSize >> 2;
	for (; aQtSize ; --aQtSize) {
		*aOut++ = ((*aIn++) ^ pKey);
	}

	pIn = reinterpret_cast<char const *>(aIn);
	pOut = reinterpret_cast<char*>(aOut);
	pSize = (pSize & 0x03);
	for (; pSize ; --pSize) {
		*pOut++ = ((*pIn++) ^ reinterpret_cast<char*>(&pKey)[pSize]);
	}
}


//================================================================

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::CMemoryShuffleAllocator
//*---------------------------------------------------------------
// DESC : Constructor
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleAllocator<TDATA>::CMemoryShuffleAllocator() : m_TickTerm(0), m_Unit(NULL), m_Lock(DF_MSDT_LOCKSIGN)
{
	// Variable for error checking
#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_OriginalValue = TDATA();
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::~CMemoryShuffleAllocator
//*---------------------------------------------------------------
// DESC : Destructor
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleAllocator<TDATA>::~CMemoryShuffleAllocator()
{
	Release();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::IsOpen
//*---------------------------------------------------------------
// DESC : Return whether allocator is available
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::IsOpen() const
{
	return(0 < m_TickTerm);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Create
//*---------------------------------------------------------------
// DESC : Initially allocate dynamic memory
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- Assumes this method is called with no existing dynamically allocated memory and no synchronization needed
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::Create()
{
	MSDT_ASSERT(NULL == m_Unit);
	MSDT_ASSERT(0 == m_TickTerm);

	char* aUnit = static_cast<char*>(malloc(sizeof(CMemoryShuffleDervUnit<TDATA>)));
	if (!aUnit) {
		m_TickTerm = 0;
		MSDT_RETURN(false);
	}

	m_Unit = new (aUnit) CMemoryShuffleDervUnit<TDATA>;		// placement new does not throw std::bad_alloc, so consequently it throws no exception ?
	MSDT_ASSERT(NULL != m_Unit);

//#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Unit->SetValue(TDATA());	// Initialized to 0
//#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	SetTickTerm();

	return true;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Create
//*---------------------------------------------------------------
// DESC : Initially allocate dynamic memory and initialize with specified value
// PARM : 1 . pData - Value to use for initialization
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- Assumes this method is called with no existing dynamically allocated memory and no synchronization needed
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::Create(const TDATA& pData)
{
	if (!Create()) {
		return false;
	}

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_OriginalValue = pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	m_Unit->SetValue(pData);

	return true;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Create
//*---------------------------------------------------------------
// DESC : Initially allocate dynamic memory and initialize with specified object
// PARM : 1 . pMsdt - Object to use for initialization
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- Assumes this method is called with no existing dynamically allocated memory and no synchronization needed
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::Create(const CMemoryShuffleDataType<TDATA>& pMsdt)
{
	if (!Create()) {
		return false;
	}

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_OriginalValue = static_cast<TDATA>(const_cast<CMemoryShuffleDataType<TDATA>&>(pMsdt));
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	m_Unit->SetValue(static_cast<TDATA>(const_cast<CMemoryShuffleDataType<TDATA>&>(pMsdt)));

	return true;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Release
//*---------------------------------------------------------------
// DESC : Free existing dynamic memory and mark allocator as unavailable
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleAllocator<TDATA>::Release()
{
	if (m_Unit) {
//		m_Unit->~TDATA(); // Explicit destructor call due to placement new (compile error for TDATA without destructor)
		
#ifdef FIX_VIOLATION_BY_FREE
		SAFE_DELETE( m_Unit );
#else
		free(reinterpret_cast<char*>(m_Unit));
		m_Unit = NULL;
#endif

		
	}
	m_TickTerm = 0;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Shuffle
//*---------------------------------------------------------------
// DESC : Convert dynamic memory to another dynamic memory at a specified time interval
// PARM : 1 . pUnit - Base unit object to perform memory shuffling
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::Shuffle(CMemoryShuffleBaseUnit<TDATA>*& pUnit) const
{
	MSDT_ASSERT(NULL != pUnit);

	// P.S.> Synchronization of the CMemoryShuffleAllocator<TDATA> object must be guaranteed upon entering this method

	CMemoryShuffleDervUnit<TDATA>* aUnit1 = static_cast<CMemoryShuffleDervUnit<TDATA>*>(pUnit);
	
	unsigned long aCurTick = ::GetTickCount();	// Okay even if resolution is low?
	if (m_TickTerm < ((aCurTick >= aUnit1->GetLastTick())?(aCurTick - aUnit1->GetLastTick()):(ULONG_MAX - aUnit1->GetLastTick() + aCurTick))) {
		TDATA aData = aUnit1->GetValue();

		char* aUnit2 = static_cast<char*>(malloc(sizeof(CMemoryShuffleDervUnit<TDATA>)));	// Allocate first to prevent getting the same address if reallocated after freeing memory
		if (!aUnit2) {
			m_TickTerm = 0;
			::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// Throw an exception on allocation failure since it should be detected immediately
			MSDT_RETURN(false);
		}

//		aUnit1->~TDATA(); // explicit destructor call due to placement new (compile error if TDATA has no destructor)
		
#ifdef FIX_VIOLATION_BY_FREE
		SAFE_DELETE( aUnit1 );
#else
		free(reinterpret_cast<char*>(aUnit1));
#endif
		
		aUnit1 = new (aUnit2) CMemoryShuffleDervUnit<TDATA>;	// placement new does not throw std::bad_alloc, so consequently it throws no exception ?
		MSDT_ASSERT(NULL != aUnit1);

		aUnit1->SetValue(aData);
		aUnit1->SetLastTick(aCurTick);

		pUnit = aUnit1;
	}

	return true;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::GetTickTerm
//*---------------------------------------------------------------
// DESC : Return existing memory shuffling interval
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline unsigned long CMemoryShuffleAllocator<TDATA>::GetTickTerm() const
{
	return m_TickTerm;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::SetTickTerm
//*---------------------------------------------------------------
// DESC : Set new memory shuffling interval
// PARM : 1 . pTickTerm - Memory shuffling interval
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleAllocator<TDATA>::SetTickTerm(unsigned long pTickTerm)
{
	MSDT_ASSERT(0 < pTickTerm);

	m_TickTerm = pTickTerm;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Lock
//*---------------------------------------------------------------
// DESC : Switch allocator to locked state
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleBaseUnit<TDATA>* CMemoryShuffleAllocator<TDATA>::Lock()
{
	long aLock = 0;

	do {
		aLock = InterlockedExchange(&m_Lock, 0);
	} while(DF_MSDT_LOCKSIGN != aLock);


	CMemoryShuffleDervUnit<TDATA>* aUnit = const_cast<CMemoryShuffleDervUnit<TDATA>*>(static_cast<CMemoryShuffleDervUnit<TDATA>*>(m_Unit));
	MSDT_ASSERT(NULL != aUnit);

	m_Unit = NULL;	// Not strictly required to specify, but intended to slightly improve security

	return aUnit;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Unlock
//*---------------------------------------------------------------
// DESC : Release allocator from locked state
// PARM : 1 . pUnit - Base unit object used to release lock state
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleAllocator<TDATA>::Unlock(const CMemoryShuffleBaseUnit<TDATA>* pUnit) const
{
	MSDT_ASSERT(NULL == m_Unit);				// Check if m_Unit is specified as NULL on Lock
	MSDT_ASSERT(NULL != pUnit);

	if (!pUnit) {
		MSDT_RETURN_NONE;
	}


	m_Unit = const_cast<CMemoryShuffleDervUnit<TDATA>*>(static_cast<const CMemoryShuffleDervUnit<TDATA>*>(pUnit));	// Since pointer swap atomicity (InterlockedExchangePointer) is no longer guaranteed as before, swap the pointer before releasing synchronization

	long aLock = InterlockedExchange(&m_Lock, DF_MSDT_LOCKSIGN);
	MSDT_ASSERT(DF_MSDT_LOCKSIGN != aLock);
}


//================================================================

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleSyncAuto::CMemoryShuffleSyncAuto
//*---------------------------------------------------------------
// DESC : Constructor
// PARM : 1 . pAlloc - TALLOC class object pointer
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TALLOC, typename TUNIT>
inline CMemoryShuffleSyncAuto<TALLOC, TUNIT>::CMemoryShuffleSyncAuto(TALLOC* pAlloc) : m_Alloc(pAlloc), m_Unit(NULL)
{
	MSDT_ASSERT(NULL != m_Alloc);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleSyncAuto::~CMemoryShuffleSyncAuto
//*---------------------------------------------------------------
// DESC : Destructor
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TALLOC, typename TUNIT>
inline CMemoryShuffleSyncAuto<TALLOC, TUNIT>::~CMemoryShuffleSyncAuto()
{
	if (m_Alloc && m_Unit) {
		m_Alloc->Unlock(m_Unit);
		m_Alloc = NULL;
		m_Unit = NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleSyncAuto::Lock
//*---------------------------------------------------------------
// DESC : Transition held allocator to locked state
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TALLOC, typename TUNIT>
inline TUNIT* CMemoryShuffleSyncAuto<TALLOC, TUNIT>::Lock()
{
	MSDT_ASSERT(NULL != m_Alloc);
	MSDT_ASSERT(NULL == m_Unit);

	m_Unit = m_Alloc->Lock();
	if (!m_Unit) {
		MSDT_RETURN(NULL);
	}

	// P.S.> Synchronization is guaranteed after LOCK in this method; on contention, waiting threads attempting LOCK may increase CPU usage, so process as fast as possible and UNLOCK

	if (!m_Alloc->Shuffle(m_Unit)) {
		MSDT_RETURN(NULL);
	}

	return m_Unit;
}


//================================================================

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : Constructor
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::CMemoryShuffleDataType()
{
	if (!m_Allocator.Create()) {
		::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// If allocation fails in the constructor, no separate error handling is possible, so throw an exception
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : Constructor
// PARM : 1 . pData - Value to use for initialization
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::CMemoryShuffleDataType(const TDATA& pData)
{
	if (!m_Allocator.Create(pData)) {
		::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// If allocation fails in the constructor, no separate error handling is possible, so throw an exception
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : Constructor
// PARM : 1 . pMsdt - Object to use for initialization
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::CMemoryShuffleDataType(const CMemoryShuffleDataType<TDATA>& pMsdt)
{
	if (!m_Allocator.Create(pMsdt)) {
		::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// If allocation fails in the constructor, no separate error handling is possible, so throw an exception
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::~CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : Destructor
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::~CMemoryShuffleDataType()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::GetAllocator
//*---------------------------------------------------------------
// DESC : Return reference to allocator object
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleAllocator<TDATA>& CMemoryShuffleDataType<TDATA>::GetAllocator()
{
	return m_Allocator;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::GetAllocTickTerm
//*---------------------------------------------------------------
// DESC : Return existing memory shuffling interval
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline unsigned long CMemoryShuffleDataType<TDATA>::GetAllocTickTerm() const
{
	m_Allocator.GetTickTerm();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::SetAllocTickTerm
//*---------------------------------------------------------------
// DESC : Set new memory shuffling interval
// PARM : 1 . pTickTerm - Memory shuffling interval
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleDataType<TDATA>::SetAllocTickTerm(unsigned long pTickTerm)
{
	m_Allocator.SetTickTerm(pTickTerm);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator TDATA
//*---------------------------------------------------------------
// DESC : Implement type conversion operator
// PARM : N/A
// RETV : current held value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::operator TDATA const () const
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	return(aUnit->GetValue());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator=
//*---------------------------------------------------------------
// DESC : Assignment operator implementation
// PARM : 1 . pData - Assignment operand value
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue = pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(pData);

	return pData;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator=
//*---------------------------------------------------------------
// DESC : Assignment operator implementation
// PARM : 1 . pMsdt - Assignment operand object
// RETV : returns object reference
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>& CMemoryShuffleDataType<TDATA>::operator=(const CMemoryShuffleDataType<TDATA>& pMsdt)
{
	TDATA aData = static_cast<TDATA>(const_cast<CMemoryShuffleDataType<TDATA>&>(pMsdt));

	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(*this);
	}

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue = aData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator++
//*---------------------------------------------------------------
// DESC : Implement ++ (prefix) increment operator
// PARM : N/A
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator++()
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() + 1;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	++m_Allocator.m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator++
//*---------------------------------------------------------------
// DESC : Implement ++ (prefix) increment operator (specialized for bool type)
// PARM : N/A
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for bool data type, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator++()
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator++
//*---------------------------------------------------------------
// DESC : Implement ++ (postfix) increment operator
// PARM : N/A
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator++(int)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue();

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	++m_Allocator.m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData + 1);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator++
//*---------------------------------------------------------------
// DESC : Implement ++ (postfix) increment operator (specialized for bool type)
// PARM : N/A
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for bool data type, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator++(int)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator--
//*---------------------------------------------------------------
// DESC : Implement -- (prefix) decrement operator
// PARM : N/A
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator--()
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() - 1;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	--m_Allocator.m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator--
//*---------------------------------------------------------------
// DESC : Implement -- (prefix) decrement operator (specialized for bool type)
// PARM : N/A
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for bool data type, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator--()
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator--
//*---------------------------------------------------------------
// DESC : Implement -- (postfix) decrement operator
// PARM : N/A
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator--(int)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue();

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	--m_Allocator.m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData - 1);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator--
//*---------------------------------------------------------------
// DESC : Implement -- (postfix) decrement operator (specialized for bool type)
// PARM : N/A
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for bool data type, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator--(int)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator+=
//*---------------------------------------------------------------
// DESC : Implement += compound assignment operator
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator+=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() + pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue += pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator+=
//*---------------------------------------------------------------
// DESC : Implement += compound assignment operator (specialized for bool type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for bool data type, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator+=(const bool& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator-=
//*---------------------------------------------------------------
// DESC : Implement -= compound assignment operator
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator-=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() - pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue -= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator-=
//*---------------------------------------------------------------
// DESC : Implement -= compound assignment operator (specialized for bool type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for bool data type, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator-=(const bool& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator*=
//*---------------------------------------------------------------
// DESC : Implement *= compound assignment operator
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator*=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() * pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue *= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator*=
//*---------------------------------------------------------------
// DESC : Implement *= compound assignment operator (specialized for bool type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for bool data type, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator*=(const bool& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator/=
//*---------------------------------------------------------------
// DESC : Implement /= compound assignment operator
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator/=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() / pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue /= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator/=
//*---------------------------------------------------------------
// DESC : Implement /= compound assignment operator (specialized for bool type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for bool data type, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator/=(const bool& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator%=
//*---------------------------------------------------------------
// DESC : Implement %= compound assignment operator
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator%=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() % pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue %= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator%=
//*---------------------------------------------------------------
// DESC : Implement %= compound assignment operator (specialized for bool type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for bool data type, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator%=(const bool& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator%=
//*---------------------------------------------------------------
// DESC : Implement %= compound assignment operator (specialized for float type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline float CMemoryShuffleDataType<float>::operator%=(const float& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(float());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator%=
//*---------------------------------------------------------------
// DESC : Implement %= compound assignment operator (specialized for double type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline double CMemoryShuffleDataType<double>::operator%=(const double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(double());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator%=
//*---------------------------------------------------------------
// DESC : Implement %= compound assignment operator (specialized for long double type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline long double CMemoryShuffleDataType<long double>::operator%=(const long double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(long double());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator&=
//*---------------------------------------------------------------
// DESC : Implement &= compound assignment operator
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator&=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() & pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue &= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator&=
//*---------------------------------------------------------------
// DESC : Implement &= compound assignment operator (specialized for float type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline float CMemoryShuffleDataType<float>::operator&=(const float& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(float());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator&=
//*---------------------------------------------------------------
// DESC : Implement &= compound assignment operator (specialized for double type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline double CMemoryShuffleDataType<double>::operator&=(const double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(double());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator&=
//*---------------------------------------------------------------
// DESC : Implement &= compound assignment operator (specialized for long double type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline long double CMemoryShuffleDataType<long double>::operator&=(const long double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(long double());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator|=
//*---------------------------------------------------------------
// DESC : Implement |= compound assignment operator
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator|=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() | pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue |= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator|=
//*---------------------------------------------------------------
// DESC : Implement |= compound assignment operator (specialized for float type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline float CMemoryShuffleDataType<float>::operator|=(const float& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(float());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator|=
//*---------------------------------------------------------------
// DESC : Implement |= compound assignment operator (specialized for double type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline double CMemoryShuffleDataType<double>::operator|=(const double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(double());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator|=
//*---------------------------------------------------------------
// DESC : Implement |= compound assignment operator (specialized for long double type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline long double CMemoryShuffleDataType<long double>::operator|=(const long double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(long double());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator^=
//*---------------------------------------------------------------
// DESC : Implement ^= compound assignment operator
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator^=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() ^ pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue ^= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator^=
//*---------------------------------------------------------------
// DESC : Implement ^= compound assignment operator (specialized for float type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline float CMemoryShuffleDataType<float>::operator^=(const float& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(float());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator^=
//*---------------------------------------------------------------
// DESC : Implement ^= compound assignment operator (specialized for double type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline double CMemoryShuffleDataType<double>::operator^=(const double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(double());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator^=
//*---------------------------------------------------------------
// DESC : Implement ^= compound assignment operator (specialized for long double type)
// PARM : 1 . pData - Operation argument value
// RETV : operation result value
// PRGM : B4nFter
// P.S.>
//		- Operation not originally supported for floating-point data types, so returns default value only (do not use this operation)
//*---------------------------------------------------------------
/* export */ template<>
inline long double CMemoryShuffleDataType<long double>::operator^=(const long double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(long double());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY
