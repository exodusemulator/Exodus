namespace Stream {

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
Hex::Hex(unsigned int length)
:_length(length)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Character length functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int Hex::GetLength() const
{
	return _length;
}

//----------------------------------------------------------------------------------------------------------------------
void Hex::SetLength(unsigned int length)
{
	_length = length;
}

//----------------------------------------------------------------------------------------------------------------------
// Stream operators
//----------------------------------------------------------------------------------------------------------------------
ViewText& operator>>(ViewText& stream, const Hex& object)
{
	return stream >> std::hex;
}

//----------------------------------------------------------------------------------------------------------------------
ViewText& operator<<(ViewText& stream, const Hex& object)
{
	return stream << std::setw(object.GetLength()) << std::setfill(L'0') << std::hex << std::uppercase;
}

} // Close namespace Stream
