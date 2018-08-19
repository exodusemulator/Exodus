//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void LogEntry::SetText(const std::wstring& text)
{
	_text.str() = L"";
	_text << text;
}

//----------------------------------------------------------------------------------------
void LogEntry::SetSource(const std::wstring& source)
{
	_source = source;
}

//----------------------------------------------------------------------------------------
void LogEntry::SetEventLevel(EventLevel level)
{
	_eventLevel = level;
}

//----------------------------------------------------------------------------------------
//Text-based stream functions
//----------------------------------------------------------------------------------------
template<class T> LogEntry& LogEntry::operator>>(T& data)
{
	_text << data;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> LogEntry& LogEntry::operator<<(const T& data)
{
	_text << data;
	return *this;
}
