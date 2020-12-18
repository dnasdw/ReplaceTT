#include <sdw.h>

bool OrigCompare(const pair<wstring, wstring>& lhs, const pair<wstring, wstring>& rhs)
{
	return lhs.first.size() > rhs.first.size();
}

int UMain(int argc, UChar* argv[])
{
	if (argc != 4)
	{
		return 1;
	}
	vector<pair<wstring, wstring>> vReplacement;
	FILE* fp = UFopen(argv[3], USTR("rb"));
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uTxtSize = ftell(fp);
	if (uTxtSize % 2 != 0)
	{
		fclose(fp);
		UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[3]);
		UPrintf(USTR("NOT Unicode(UTF-16 LE) txt\n"));
		return 1;
	}
	uTxtSize /= 2;
	fseek(fp, 0, SEEK_SET);
	Char16_t* pTemp = new Char16_t[uTxtSize + 1];
	fread(pTemp, 2, uTxtSize, fp);
	fclose(fp);
	if (pTemp[0] != 0xFEFF)
	{
		delete[] pTemp;
		UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[3]);
		UPrintf(USTR("NOT Unicode(UTF-16 LE) txt\n"));
		return 1;
	}
	pTemp[uTxtSize] = 0;
	wstring sTxt = U16ToW(pTemp + 1);
	delete[] pTemp;
	if (!sTxt.empty())
	{
		vector<wstring> vTxt = SplitOf(sTxt, L"\r\n");
		for (n32 i = 0; i < static_cast<n32>(vTxt.size()); i++)
		{
			const wstring& sLine = vTxt[i];
			if (sLine.empty())
			{
				continue;
			}
			vector<wstring> vLine = Split(sLine, L"=");
			for (n32 j = 2; j < static_cast<n32>(vLine.size()); j++)
			{
				vLine[1] += L"=" + vLine[j];
			}
			vReplacement.push_back(make_pair(vLine[0], vLine[1]));
		}
	}
	stable_sort(vReplacement.begin(), vReplacement.end(), OrigCompare);
	fp = UFopen(argv[1], USTR("rb"));
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	uTxtSize = ftell(fp);
	if (uTxtSize % 2 != 0)
	{
		fclose(fp);
		UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
		UPrintf(USTR("NOT Unicode(UTF-16 LE) txt\n"));
		return 1;
	}
	uTxtSize /= 2;
	fseek(fp, 0, SEEK_SET);
	pTemp = new Char16_t[uTxtSize + 1];
	fread(pTemp, 2, uTxtSize, fp);
	fclose(fp);
	if (pTemp[0] != 0xFEFF)
	{
		delete[] pTemp;
		UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
		UPrintf(USTR("NOT Unicode(UTF-16 LE) txt\n"));
		return 1;
	}
	pTemp[uTxtSize] = 0;
	sTxt = U16ToW(pTemp + 1);
	delete[] pTemp;
	wstring sTxtNew;
	wstring::size_type uPos0 = 0;
	while ((uPos0 = sTxt.find(L"No.", uPos0)) != wstring::npos)
	{
		wstring::size_type uPos1 = sTxt.find(L"\r\n--------------------------------------\r\n", uPos0);
		if (uPos1 == wstring::npos)
		{
			UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
			UPrintf(USTR("NOT FOUND \\r\\n--------------------------------------\\r\\n\n"));
			return 1;
		}
		wstring sNum = sTxt.substr(uPos0, uPos1 - uPos0);
		uPos0 = uPos1 + wcslen(L"\r\n--------------------------------------\r\n");
		uPos1 = sTxt.find(L"\r\n======================================\r\n", uPos0);
		if (uPos1 == wstring::npos)
		{
			UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
			UPrintf(USTR("NOT FOUND \\r\\n======================================\\r\\n\n"));
			return 1;
		}
		wstring sStmtOld = sTxt.substr(uPos0, uPos1 - uPos0);
		uPos0 = uPos1 + wcslen(L"\r\n======================================\r\n");
		uPos1 = sTxt.find(L"\r\n--------------------------------------", uPos0);
		if (uPos1 == wstring::npos)
		{
			UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
			UPrintf(USTR("NOT FOUND \\r\\n--------------------------------------\n"));
			return 1;
		}
		wstring sStmtNew = sTxt.substr(uPos0, uPos1 - uPos0);
		uPos0 = uPos1 + wcslen(L"\r\n--------------------------------------");
		wstring sTempTxt = sStmtOld;
		sTempTxt = Replace(sTempTxt, L"[--------------------------------------]", L"");
		sTempTxt = Replace(sTempTxt, L"[======================================]", L"");
		if (sTempTxt.find(L"--------------------------------------") != wstring::npos)
		{
			UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
			UPrintf(USTR("orig FOUND --------------------------------------\n"));
			return 1;
		}
		if (sTempTxt.find(L"======================================") != wstring::npos)
		{
			UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
			UPrintf(USTR("orig FOUND ======================================\n"));
			return 1;
		}
		if (sTempTxt.find(L"No.") != wstring::npos)
		{
			UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
			UPrintf(USTR("orig FOUND No.\n"));
			return 1;
		}
		sTempTxt = sStmtNew;
		sTempTxt = Replace(sTempTxt, L"[--------------------------------------]", L"");
		sTempTxt = Replace(sTempTxt, L"[======================================]", L"");
		if (sTempTxt.find(L"--------------------------------------") != wstring::npos)
		{
			UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
			UPrintf(USTR("zh FOUND --------------------------------------\n"));
			return 1;
		}
		if (sTempTxt.find(L"======================================") != wstring::npos)
		{
			UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
			UPrintf(USTR("zh FOUND ======================================\n"));
			return 1;
		}
		if (sTempTxt.find(L"No.") != wstring::npos)
		{
			UPrintf(USTR("ERROR: %") PRIUS USTR("\n"), argv[1]);
			UPrintf(USTR("zh FOUND No.\n"));
			return 1;
		}
		for (vector<pair<wstring, wstring>>::const_iterator it = vReplacement.begin(); it != vReplacement.end(); ++it)
		{
			sStmtNew = Replace(sStmtNew, it->first, it->second);
		}
		if (!sTxtNew.empty())
		{
			sTxtNew += L"\r\n\r\n";
		}
		sTxtNew += sNum;
		sTxtNew += L"\r\n--------------------------------------\r\n";
		sTxtNew += sStmtOld;
		sTxtNew += L"\r\n======================================\r\n";
		sTxtNew += sStmtNew;
		sTxtNew += L"\r\n--------------------------------------\r\n";
	}
	fp = UFopen(argv[2], USTR("wb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fwrite("\xFF\xFE", 2, 1, fp);
	fwrite(sTxtNew.c_str(), 2, sTxtNew.size(), fp);
	fclose(fp);
	return 0;
}
