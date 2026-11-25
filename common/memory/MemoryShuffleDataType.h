/*//===============================================================

	<< MEMORY-SHUFFLE DATA TYPE >>

	PRGM : B4nFter

	FILE : MEMORYSHUFFLEDATATYPE.H, MEMORYSHUFFLEDATATYPE.INL
	DESC : 
	INIT BUILT DATE : 2009. 09. 12
	LAST BUILT DATE : 2010. 07. 21

	P.S.>
- Must thoroughly read the usage and precautions described in the separate document before use

- Template-specialized so unsupported operations per basic data type listed in precautions do not cause compile errors or warnings; declare the #define below above #include to intentionally trigger compile errors or warnings

			SW_MSDT_NOTINCLUDE_COMPILESAFETY

- When using a compiler supporting the C++ 'export' keyword, .h and .inl must be split into .h and .cpp (Ref: http://www.comeaucomputing.com/4.0/docs/userman/export.html)

- Compile error occurs when using unsupported data types (2009.11.23> pointer, reference) (intentional)

- Need to add a feature that triggers link error 'error LNK2001' in preparation for restructuring this module once C++ keyword 'export' is supported

- Provides the features below for error checking on exceptions; declare the #define below above #include for this header to enable

			SW_MSDT_ERRORCHECK_ORIGINVALUE
: Maintains a duplicate variable of type TDATA and applies the same on all references and operations
: Converts shuffled variable to actual value for reference and compares with duplicate variable right before operation

- To call assert() on exceptions, declare the #define below above #include for this header

			SW_MSDT_ASSERT

- The #define below is used to inspect internal MSDT behavior; unnecessary in most cases and lowers security, so it must be removed/disabled in production

			SW_MSDT_TEST

Copyright (c) 2002~2010 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

enum EF_MSDT_ALLOCTICKTERM						// Memory allocation interval
{
	EV_MSDT_ALTT_VERYFAST		= (1*1000),		// When extreme security is required
	EV_MSDT_ALTT_FAST			= (5*1000),		// When security takes priority
	EV_MSDT_ALTT_MEDIUM			= (10*1000),	// Normal case
	EV_MSDT_ALTT_SLOW			= (30*1000),	// When performance is prioritized
	EV_MSDT_ALTT_VERYSLOW		= (60*1000),	// When extreme performance is required
};

enum EF_MSDT_EXCEPTIONCODE						// Exception code (ref: http://msdn.microsoft.com/en-us/library/het71c37.aspx)
{
	EV_MSDT_ECCD_NONE			= 0xE0000000,	// N/A
	EV_MSDT_ECCD_ALLOCFAIL		= 0xE0010000,	// Memory allocation failed
};

#if defined(PRE_MEMORY_SHUFFLE)
#define MSDT_DECL(pTYPE)			CMemoryShuffleDataType<pTYPE>	// USING MSDT
#define MSDT_SETTERM(pMSDT,pTIME)	pMSDT.SetAllocTickTerm(pTIME)	// USING MSDT
#else	// _DEBUG
#define MSDT_DECL(pTYPE)			pTYPE							// NOT-USING MSDT
#define MSDT_SETTERM(pMSDT,pTIME)									// NOT-USING MSDT
#endif	// _DEBUG

#if defined(SW_MSDT_ASSERT)
#define MSDT_ASSERT(pCOND)		if (!(pCOND)) { DebugBreak(); }
#define MSDT_RETURN(pRETV)		DebugBreak(); return(pRETV);
#define MSDT_RETURN_NONE		DebugBreak(); return;
#else	// #if defined(SW_MSDT_ASSERT)
#define MSDT_ASSERT(pCOND)		
#define MSDT_RETURN(pRETV)		return(pRETV);
#define MSDT_RETURN_NONE		return;
#endif	// #if defined(SW_MSDT_ASSERT)


///////////////////////////////////////////////////////////////////
//	FORWARD DECLARATION
///////////////////////////////////////////////////////////////////

template<typename TDATA>
class CMemoryShuffleBaseUnit;

template<typename TDATA>
class CMemoryShuffleAllocator;

template<typename TDATA>
class CMemoryShuffleDataType;


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleBaseUnit
//*---------------------------------------------------------------
// DESC : Memory shuffle basic unit class (base)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleBaseUnit
{

public:
	CMemoryShuffleBaseUnit();
	virtual ~CMemoryShuffleBaseUnit();

	virtual TDATA GetValue() const = 0;
	virtual void SetValue(TDATA pData) = 0;

	unsigned long GetLastTick() const;
	void SetLastTick(unsigned long pLastTick);
		
private:
	mutable unsigned long m_LastTick;

};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleAllocator
//*---------------------------------------------------------------
// DESC : Allocator class responsible for dynamic memory management of memory shuffle
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleAllocator
{

private:
	mutable CMemoryShuffleBaseUnit<TDATA>* m_Unit;

public:
	friend class CMemoryShuffleDataType<TDATA>;

public:
	CMemoryShuffleAllocator();
	virtual ~CMemoryShuffleAllocator();

	bool IsOpen() const;

	bool Shuffle(CMemoryShuffleBaseUnit<TDATA>*& pUnit) const;

 	CMemoryShuffleBaseUnit<TDATA>* Lock();
 	void Unlock(const CMemoryShuffleBaseUnit<TDATA>* pUnit) const;

	unsigned long GetTickTerm() const;
	void SetTickTerm(unsigned long pTickTerm = EV_MSDT_ALTT_FAST);

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	const TDATA GetOriginalValue() const { return m_OriginalValue; }
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

private:
	bool Create();
	bool Create(const TDATA& pData);
	bool Create(const CMemoryShuffleDataType<TDATA>& pMsdt);

	void Release();

private:
	mutable unsigned long m_TickTerm;

public:
	volatile mutable long m_Lock;

	// Variable for error checking
#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	TDATA m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleSyncAuto
//*---------------------------------------------------------------
// DESC : Class ensuring synchronization during memory shuffle data type operations
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TALLOC, typename TUNIT>
class CMemoryShuffleSyncAuto
{
//	DISABLE_COPY_ASSIGN(CMemoryShuffleSyncAuto) // Disallow object copy and assignment

public:
	CMemoryShuffleSyncAuto(TALLOC* pAlloc);
	~CMemoryShuffleSyncAuto();

	TUNIT* Lock();

private:
	// Disallow copy and assignment between objects
	CMemoryShuffleSyncAuto(const CMemoryShuffleSyncAuto<TALLOC, TUNIT>& pSyncAuto) : m_Alloc(pSyncAuto.m_Alloc)		{ MSDT_ASSERT(0); /* "Can't be Called!" */ }
#pragma warning (disable:4100)
	CMemoryShuffleSyncAuto& operator=(const CMemoryShuffleSyncAuto<TALLOC, TUNIT>& pSyncAuto)						{ MSDT_ASSERT(0); /* "Can't be Called!" */ return(*this); }
#pragma warning (default:4100)

private:
	TALLOC* m_Alloc;
	TUNIT* m_Unit;

};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : Memory shuffle data type class
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType
{

public:
	CMemoryShuffleDataType();
	CMemoryShuffleDataType(const TDATA& pData);
	CMemoryShuffleDataType(const CMemoryShuffleDataType<TDATA>& pMsdt);
	virtual ~CMemoryShuffleDataType();

	operator TDATA const () const;
// 	operator TDATA* const ();	// not support
// 	operator TDATA& const ();	// not support

	TDATA operator=(const TDATA& pData);
	CMemoryShuffleDataType<TDATA>& operator=(const CMemoryShuffleDataType<TDATA>& pMsdt);

	TDATA operator++();			// ++V (prefix)
	TDATA operator++(int);		// V++ (postfix)
	TDATA operator--();			// --V (pre-decrement)
	TDATA operator--(int);		// V-- (postfix)

	TDATA operator+=(const TDATA& pData);
	TDATA operator-=(const TDATA& pData);
	TDATA operator*=(const TDATA& pData);
	TDATA operator/=(const TDATA& pData);
	TDATA operator%=(const TDATA& pData);
	TDATA operator&=(const TDATA& pData);
	TDATA operator|=(const TDATA& pData);
	TDATA operator^=(const TDATA& pData);

	unsigned long GetAllocTickTerm() const;
	void SetAllocTickTerm(unsigned long pTickTerm);

#if defined(SW_MSDT_TEST)
	__forceinline const CMemoryShuffleBaseUnit<TDATA>* GetPtr() const { return(m_Allocator.m_Unit); }		// Temporary method for testing !!! - Must be removed before production use
#endif	// SW_MSDT_TEST

private:
	typedef	CMemoryShuffleSyncAuto<CMemoryShuffleAllocator<TDATA>, CMemoryShuffleBaseUnit<TDATA>>	TP_SYNCAUTO;

	CMemoryShuffleAllocator<TDATA>& GetAllocator();

private:
	mutable CMemoryShuffleAllocator<TDATA> m_Allocator;

};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : Memory shuffle data type class (pointers prohibited)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<TDATA*>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// Trigger compile-time error
	}
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : Memory shuffle data type class (pointers prohibited)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<const TDATA*>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// Trigger compile-time error
	}
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : Memory shuffle data type class (references prohibited)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<TDATA&>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// Trigger compile-time error
	}
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : Memory shuffle data type class (references prohibited)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<const TDATA&>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// Trigger compile-time error
	}
};


#include "MemoryShuffleDataType.inl"	// P.S.> Remove on compilers that support the 'export' keyword
