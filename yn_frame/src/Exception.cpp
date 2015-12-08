#include "Exception.h"

namespace utility{
CException::CException(int iErrorNo, const char* sErrorMsg, const char* sFile, int iLine)
{
	m_iErrorNo = iErrorNo;
	if (sErrorMsg)
		m_strErrorMsg = sErrorMsg;
	if (sFile)
		m_strFile = sFile;
	m_iLine = iLine;
}


CException::CException(const char* sErrorMsg, const char* sFile, int iLine)
{
	m_iErrorNo = EMPTY_ERROR_NO;
	if (sErrorMsg)
		m_strErrorMsg = sErrorMsg;
	if (sFile)
		m_strFile = sFile;
	m_iLine = iLine;
}


ostream& operator << (ostream& os, const CException& e)
{
	if (e.m_iErrorNo != CException::EMPTY_ERROR_NO)
		os << e.m_iErrorNo << ":";
		
	os << e.m_strErrorMsg;
	
	if (!e.m_strFile.empty() && e.m_iLine != CException::EMPTY_ERROR_NO)
		os << " [" << e.m_strFile << ":" << e.m_iLine << "]";
		
	return os;
}


ostream& operator << (ostream& os, CMemException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CFileException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CFileLockException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CHtmlException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CSemException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CShmException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CSocketException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CThreadException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CNBTcpSvrException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CPreforkSvrException& e)
{
	return os << (CException&)e;
}

ostream& operator << (ostream& os, CUnsupportedException& e)
{
	return os << (CException&)e;
}

ostream& operator << (ostream& os, CInvalidInputException& e)
{
	return os << (CException&)e;
}

ostream& operator << (ostream& os, CEmptyException& e)
{
	return os << (CException&)e;
}
}

