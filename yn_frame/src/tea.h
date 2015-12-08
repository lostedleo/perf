/*
 * =====================================================================================
 *
 *       Filename:  tea.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年12月04日 11时23分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhu Zhenwei (Losted_leo), losted.leo@gmail.com
 *        Company:  Flashget, Inc.
 *
 * =====================================================================================
 */

#ifndef _tea_h_99B8C39C_DCE2_44aa_BC78_149CFB581967_
#define _tea_h_99B8C39C_DCE2_44aa_BC78_149CFB581967_

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <assert.h>

using namespace std;

// ****************************************************************************
// * function defines
// ****************************************************************************

// ----------------------------------------------------------------------------
// Function   : allocate memory with/without alignment
// Parameters :
//      [in ] : iSize  - size of the memory to allocated
//            : iAlign - number of bytes aligned
//      [out] : none
// Return     : none zero for success, otherwise (NULL) failed
// ----------------------------------------------------------------------------
void* align_alloc_memory(int32_t iSize, int32_t iAlign);

// ----------------------------------------------------------------------------
// Function   : free memory allocated by xcoder_alloc_memory
// Parameters :
//      [in ] : p - pointer to the memory allocated by xcoder_alloc_memory
//      [out] : none
// Return     : void
// ----------------------------------------------------------------------------
void align_free_memory(void* p);

#ifdef __cplusplus
extern "C" {                          // only need to export C interface if used by C++ source code
#endif

	void tea_encrypt_sse(uint32_t *pIn, uint32_t *pOut, uint32_t uiLen, uint32_t *pKey, uint32_t *pDelta, uint32_t round);
	void tea_decrypt_sse(uint32_t *pIn, uint32_t *pOut, uint32_t uiLen, uint32_t *pKey, uint32_t *pDelta, uint32_t *pSum, uint32_t round);

#ifdef __cplusplus
}
#endif

#define SAFE_ALIGN_FREE(ptr)				\
	if (ptr)								\
	{										\
		align_free_memory((void *)ptr);		\
		ptr = NULL;							\
	}
	

class CMyTea
{
public:
	CMyTea(const string& sKey, bool bIfPadding=true, uint32_t uiRound=16, uint32_t uiDelta=0x9E3779B9);
	virtual ~CMyTea();

	int teaEncrypt(uint8_t* pIn, uint64_t& ui64Len, uint8_t* pOut);
	int teaDecrypt(uint8_t* pIn, uint64_t& ui64Len, uint8_t* pOut);

private:
	void tea_encrypt(const uint32_t* pIn, uint32_t* pOut);	
	void tea_decrypt(const uint32_t* pIn, uint32_t* pOut);
	uint32_t encPadding(const uint8_t* pIn, uint32_t uiRestLen, uint8_t* pOut);

protected:
	uint32_t	m_uiRound;
	uint32_t	m_uiDelta;
	bool		m_bIfPadding;
	string		m_sKey;
};

class CTeaEx:public CMyTea
{
public:
	CTeaEx(const string& sKey, bool bIfPadding=true, uint32_t uiRound=16, uint32_t uiDelta=0x9E3779B9);
	virtual ~CTeaEx();

public:
	int teaEncryptEx(uint8_t* pIn, uint64_t& ui64Len, uint8_t* pOut);
	int teaDecryptEx(uint8_t* pIn, uint64_t& ui64Len, uint8_t* pOut);

private:
	void init(const string& sKey, uint32_t uiRound=16, uint32_t uiDelta=0x9E3779B9);

private:
	uint32_t	*m_pKey;
	uint32_t	*m_pDelta;
	uint32_t	*m_pSum;
};

#endif

