#include "Direct3DException.h"
#include <sstream>



Direct3DException::Direct3DException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* Direct3DException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Direct3DException::GetType() const noexcept
{
	return "Direct Exception";
}

int Direct3DException::GetLine() const noexcept
{
	return line;
}

const std::string& Direct3DException::GetFile() const noexcept
{
	return file;
}

std::string Direct3DException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File]" << file << std::endl
		<< "[Line" << line;
	return oss.str();
}