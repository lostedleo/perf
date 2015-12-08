/*
 * =====================================================================================
 *
 *       Filename:  tea.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年12月04日 11时25分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhu Zhenwei (Losted_leo), losted.leo@gmail.com
 *        Company:  Flashget, Inc.
 *
 * =====================================================================================
 */

#include "tea.h"

#define TEA_KEY_LENGTH				16
#define TEA_ENCRYPT_BLOCK_SIZE		8
#define BASE_SSE_ENCRYPT_SIZE		32

void* align_alloc_memory(int32_t iSize, int32_t iAlign)
{
	uint8_t* pMem;
	assert(iAlign >= 0 && iAlign < 256);

	if (iAlign > 0)
	{
		uint8_t* pTmp;

		// allocate the required size memory + alignment,
		// so we can realign the data if necessary
		if ((pTmp = (uint8_t *)malloc(iSize + iAlign)) != NULL)
		{
			// align the pTmp point32_ter
			pMem = (uint8_t *)((uint64_t)(pTmp + iAlign - 1) & (~(uint64_t)(iAlign - 1)));

			// special case where malloc have already satisfied the alignment,
			// we must add alignment to pMem because we must store
			// (pMem - pTmp) in *(pMem-1)
			if (pMem == pTmp)
			{
				pMem += iAlign;
			}

			// (pMem - pTmp) is stored in *(pMem-1), so we are able to retrieve
			// the real malloc block allocated and free it in xcoder_free_memory
			*(pMem - 1) = (uint8_t)(pMem - pTmp);

			// return the aligned point32_ter
			return ((void *)pMem);
		}
	}
	else
	{
		// no any alignment
		if ((pMem = (uint8_t *)malloc(iSize + 1)) != NULL)
		{
			// store 1 in *(pMem)
			*pMem = (uint8_t)1;

			// return the point32_ter
			return ((void *)(pMem+1));
		}
	}

	return NULL;
}

void align_free_memory(void* p)
{
	uint8_t* pTmp = (uint8_t*)p;

	if (pTmp != NULL)
	{
		// *(pTmp - 1) holds the offset to the real allocated block
		// sub the offset, and then free the real point32_ter
		pTmp -= *(pTmp - 1);

		// free the memory
		free(pTmp);
	}
}

/********************************* calss CTea  *****************************************/
CMyTea::CMyTea(const string& sKey, bool bIfPadding/* =true */, uint32_t uiRound/* =16 */, uint32_t uiDelta/* =0x9E3779B9 */)
{
	m_sKey = sKey;
	m_bIfPadding = bIfPadding;
	m_uiRound = uiRound;
	m_uiDelta = uiDelta;
}

CMyTea::~CMyTea()
{
}

int CMyTea::teaEncrypt(uint8_t* pIn, uint64_t& ui64Len, uint8_t* pOut)
{
	if ((NULL == pIn) || (NULL == pOut) || (m_sKey.length() != TEA_KEY_LENGTH))
	{
		return -1;
	}
	if (!m_bIfPadding && (ui64Len % TEA_ENCRYPT_BLOCK_SIZE != 0))
	{
		return -1;
	}

	uint64_t ui64InOffset=0, ui64OutOffset=0;
	while(ui64Len - ui64InOffset >= TEA_ENCRYPT_BLOCK_SIZE)
	{
		tea_encrypt((const uint32_t*)(pIn + ui64InOffset), (uint32_t*)(pOut + ui64OutOffset));
		ui64InOffset += TEA_ENCRYPT_BLOCK_SIZE;
		ui64OutOffset += TEA_ENCRYPT_BLOCK_SIZE;
	}

	if (m_bIfPadding)
	{
		ui64OutOffset += encPadding(pIn+ui64InOffset, (uint32_t)(ui64Len-ui64InOffset), pOut+ui64OutOffset);
	}
	ui64Len = ui64OutOffset;

	return 0;
}

int CMyTea::teaDecrypt(uint8_t* pIn, uint64_t& ui64Len, uint8_t* pOut)
{
	if ((NULL == pIn) || (NULL == pOut) || (m_sKey.length() != TEA_KEY_LENGTH))
	{
		return -1;
	}
	if (ui64Len % TEA_ENCRYPT_BLOCK_SIZE != 0)
	{
		return -1;
	}

	int64_t ui64InOffset=0, ui64OutOffset=0;
	while(ui64Len - ui64InOffset > 0)
	{
		tea_decrypt((const uint32_t*)(pIn + ui64InOffset), (uint32_t*)(pOut + ui64OutOffset));
		ui64InOffset += TEA_ENCRYPT_BLOCK_SIZE;
		ui64OutOffset += TEA_ENCRYPT_BLOCK_SIZE;
	}

	if (m_bIfPadding)
	{
		uint8_t ui8Padding = *(uint8_t *)(pOut + ui64OutOffset - 1);
		if (ui8Padding > TEA_ENCRYPT_BLOCK_SIZE)
		{
			return -1;
		}
		ui64OutOffset -= ui8Padding;
	}
	ui64Len = ui64OutOffset;

	return 0;
}

void CMyTea::tea_encrypt(const uint32_t* pIn, uint32_t* pOut)
{
	uint32_t* key_array = (uint32_t*)m_sKey.c_str();
	register uint32_t key_0 = key_array[0];
	register uint32_t key_1 = key_array[1];
	register uint32_t key_2 = key_array[2];
	register uint32_t key_3 = key_array[3];

	register uint32_t in_0 = pIn[0];
	register uint32_t in_1 = pIn[1];

	register uint32_t round = m_uiRound;
	register uint32_t delta = m_uiDelta;
	register uint32_t sum = 0;

	while(round--)
	{
		sum += delta;
		in_0 += ((in_1 << 4) + key_0) ^ (in_1 + sum) ^ ((in_1 >> 5) + key_1);
		in_1 += ((in_0 << 4) + key_2) ^ (in_0 + sum) ^ ((in_0 >> 5) + key_3);
	}

	pOut[0] = in_0;
	pOut[1] = in_1;
}

void CMyTea::tea_decrypt(const uint32_t* pIn, uint32_t* pOut)
{
	uint32_t* key_array = (uint32_t*)m_sKey.c_str();
	register uint32_t key_0 = key_array[0];
	register uint32_t key_1 = key_array[1];
	register uint32_t key_2 = key_array[2];
	register uint32_t key_3 = key_array[3];

	register uint32_t in_0 = pIn[0];
	register uint32_t in_1 = pIn[1];

	register uint32_t round = m_uiRound;
	register uint32_t delta = m_uiDelta;
	register uint32_t sum = m_uiDelta*m_uiRound;

	while(round--)
	{
		in_1 -= ((in_0 << 4) + key_2) ^ (in_0 + sum) ^ ((in_0 >> 5) + key_3);
		in_0 -= ((in_1 << 4) + key_0) ^ (in_1 + sum) ^ ((in_1 >> 5) + key_1);
		sum -= delta;
	}

	pOut[0] = in_0;
	pOut[1] = in_1;
}

uint32_t CMyTea::encPadding(const uint8_t* pIn, uint32_t uiRestLen, uint8_t* pOut)
{
	uint8_t inBuf[TEA_ENCRYPT_BLOCK_SIZE], outBuf[TEA_ENCRYPT_BLOCK_SIZE];
	memset(inBuf, 0, sizeof(inBuf));
	memset(outBuf, 0, sizeof(outBuf));

	uint32_t uiFillLen = TEA_ENCRYPT_BLOCK_SIZE - uiRestLen;;
	if (uiRestLen)
	{
		memcpy(inBuf, pIn, uiRestLen);
	}
	memset(inBuf+uiRestLen, uiFillLen, uiFillLen);
	tea_encrypt((const uint32_t*)inBuf, (uint32_t *)pOut);

	return uiFillLen+uiRestLen;
}

/********************************* calss CTeaEx  ***************************************/
CTeaEx::CTeaEx(const string& sKey, bool bIfPadding/* =true */, uint32_t uiRound/* =16 */, uint32_t uiDelta/* =0x9E3779B9 */)
	:CMyTea(sKey, bIfPadding, uiRound, uiDelta)
{
	init(sKey, uiRound, uiDelta);
}

CTeaEx::~CTeaEx()
{
	SAFE_ALIGN_FREE(m_pKey);
	SAFE_ALIGN_FREE(m_pDelta);
	SAFE_ALIGN_FREE(m_pSum);
}

int CTeaEx::teaEncryptEx(uint8_t* pIn, uint64_t& ui64Len, uint8_t* pOut)
{
	if ((NULL == pIn) || (NULL == pOut) || (m_sKey.length() != TEA_KEY_LENGTH))
	{
		return -1;
	}
	if (!m_bIfPadding && (ui64Len % TEA_ENCRYPT_BLOCK_SIZE != 0))
	{
		return -1;
	}

	uint64_t uiFirstEncLen = (ui64Len>>5)<<5;
	if (uiFirstEncLen)
	{
		tea_encrypt_sse((uint32_t *)pIn, (uint32_t *)pOut, (uint32_t)uiFirstEncLen, m_pKey, m_pDelta, m_uiRound);
	}
	uint64_t uiRestLen = ui64Len - uiFirstEncLen;
	CMyTea::teaEncrypt(pIn+uiFirstEncLen, uiRestLen, pOut+uiFirstEncLen);
	ui64Len = uiFirstEncLen + uiRestLen;

	return 0;
}

int CTeaEx::teaDecryptEx(uint8_t* pIn, uint64_t& ui64Len, uint8_t* pOut)
{
	if ((NULL == pIn) || (NULL == pOut) || (m_sKey.length() != TEA_KEY_LENGTH))
	{
		return -1;
	}
	if (ui64Len % TEA_ENCRYPT_BLOCK_SIZE != 0)
	{
		return -1;
	}

	uint64_t uiFirstDecLen = (ui64Len>>5)<<5;
	if (uiFirstDecLen)
	{
		tea_decrypt_sse((uint32_t *)pIn, (uint32_t *)pOut, (uint32_t)uiFirstDecLen, m_pKey, m_pDelta, m_pSum, m_uiRound);
	}
	uint64_t uiRestLen = ui64Len - uiFirstDecLen;
	if (uiRestLen)
	{
		CMyTea::teaDecrypt(pIn+uiFirstDecLen, uiRestLen, pOut+uiFirstDecLen);
	}
	else
	{
		if (m_bIfPadding)
		{
			uint8_t ui8Padding = *(uint8_t *)(pOut + uiFirstDecLen - 1);
			if (ui8Padding > TEA_ENCRYPT_BLOCK_SIZE)
			{
				return -1;
			}
			uiFirstDecLen -= ui8Padding;
		}
	}
	ui64Len = uiFirstDecLen + uiRestLen;

	return 0;
}

/***************************************************
 *pKey(k0, k0, k0, k0, k1, k1, k1, k1, k2, k2, k2, k2, k3, k3, k3, k3)
 *pDelta(delta 4 times)
 *pSum(sum 4 times)
 ***************************************************/
void CTeaEx::init(const string& sKey, uint32_t uiRound/* =16 */, uint32_t uiDelta/* =0x9E3779B9 */)
{
	m_pKey = (uint32_t *)align_alloc_memory(sKey.length()*4, 16);
	m_pDelta = (uint32_t *)align_alloc_memory(sizeof(uint32_t)*4, 16);
	m_pSum = (uint32_t *)align_alloc_memory(sizeof(uint32_t)*4, 16);
	uint32_t uiSum = uiDelta*m_uiRound;

	for (uint32_t i=0; i<4; ++i)
	{
		for (uint32_t j=0; j<4; j++)
		{
			memcpy((uint32_t *)m_pKey + 4*i + j, (uint32_t *)(m_sKey.c_str() + i*4), 4);
		}
		memcpy((uint32_t *)m_pDelta + i, &uiDelta, 4);
		memcpy((uint32_t *)m_pSum + i, &uiSum, 4);
	}
}


