#pragma once


class CStyle
{
public:
	CStyle(HANDLE hConsole, int nColor)
	{
		m_hConsole = hConsole;
		SetConsoleTextAttribute(m_hConsole, nColor);
	}

	~CStyle()
	{
		SetConsoleTextAttribute(m_hConsole, 7);
	}

private:
	HANDLE m_hConsole;
};

