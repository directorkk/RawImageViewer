#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <math.h>

#define M_PI 3.14159265358979323846
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))

namespace Memory
{
	template <class T> void SafeDelete(T& t)
	{
		if(t)
		{
			delete t;
			t = nullptr;
		}
	}

	template <class T> void SafeDeleteArr(T& t)
	{
		if(t)
		{
			delete [] t;
			t = nullptr;
		}
	}

	template <class T> void SafeRelease(T& t)
	{
		if(t)
		{
			t->Release();
			t = nullptr;
		}
	}
}

class Util{
public:

	Util();
	~Util();

	static bool InTheRange( double subject, double range1, double range2 )
	{
		double max, min;

		if( range1 >= range2 ){
			max = range1;
			min = range2;
		}
		else{
			max = range2;
			min = range1;
		}

		if( subject <= max && subject >= min )
			return true;
		else
			return false;
	}
	static std::string wstos(const wchar_t* str)
	{
		std::wstring wstr = str;
		unsigned int len = wstr.length() * 4;
		setlocale(LC_CTYPE, "");
		char *p = new char[len+1];
		size_t charConvert = 0;
		wcstombs_s(&charConvert, p, len+1, wstr.c_str(), len);

		return std::string(p);
	}
	static wchar_t* stows(const char* str)
	{
		wchar_t* tmpWs = new wchar_t[4096];
		MultiByteToWideChar(CP_ACP, 0, str, -1, tmpWs, 4096);

		return tmpWs;
	}
	// 'digit' means the zero-stuffing, 0 for without it
	static std::vector<unsigned char> uitobitdecimal(int bit,unsigned int val, unsigned short digit = 0)
	{
		std::vector<unsigned char> output;
		std::vector<unsigned int> pow_result;

		for(int i=0 ; ; i++){
			pow_result.push_back((unsigned int)pow((double)bit, (double)i));
			if(pow_result[pow_result.size()-1] > val){
				pow_result.pop_back();

				for(int j=(int)pow_result.size()-1 ; j>=0 ; j--){
					output.push_back(val/pow_result[j]);
					val %= (unsigned int)pow_result[j];
				}

				break;
			}
		}
		if(digit > output.size()){
			unsigned int s = output.size();
			for(unsigned int i=0 ; i<digit-s ; i++){
				output.insert(output.begin(), 0);
			}
		}

		return output;
	}
	static std::vector<unsigned char> combineByte(std::vector<unsigned char*> bytes, std::vector<unsigned int> bSizes, int reserve_len = -1)
	{
		std::vector<unsigned char> output;

		if(reserve_len != -1){
			output.reserve(reserve_len);
		}
		for(unsigned int i=0 ; i<bytes.size() ; i++){
			output.insert(output.end(), bytes[i], bytes[i]+bSizes[i]);

			//delete bytes[i];
		}

		return output;
	}
	static std::vector<unsigned char> combineByte(std::vector<unsigned char*> bytes, std::vector<unsigned int> bSizes, std::vector<unsigned int> order, int reserve_len = -1)
	{
		std::vector<unsigned char> output;

		int sumSize = 0;
		for(unsigned int i=0 ; i<bSizes.size() ; i++)
			sumSize += bSizes[i];
		reserve_len = sumSize;

		if(reserve_len != -1)
			output.reserve(reserve_len);

		if(order.size() == bytes.size()){
			std::vector<unsigned int> index;
			index.resize(order.size());
			for(unsigned int i=0 ; i<order.size() ; i++)
				index[order[i]] = i;
			for(unsigned int i=0 ; i<index.size() ; i++)
				output.insert(output.end(), bytes[index[i]], bytes[index[i]]+bSizes[index[i]]);

			return output;
		}

		for(unsigned int i=0 ; i<bytes.size() ; i++){
			output.insert(output.end(), bytes[i], bytes[i]+bSizes[i]);
		}

		return output;
	}
	// * memory should be free by user
	static unsigned char* combineBytePointer(std::vector<unsigned char*> bytes, std::vector<unsigned int> bSizes, std::vector<unsigned int> order, int &bufferSize)
	{
		int sumSize = 0;
		for(unsigned int i=0 ; i<bSizes.size() ; i++)
			sumSize += bSizes[i];
		bufferSize = sumSize;

		unsigned char *output = new unsigned char[sumSize];
		
		if(order.size() == bytes.size()){
			std::vector<unsigned int> index;
			index.resize(order.size());
			int sumSize = 0;
			for(unsigned int i=0 ; i<order.size() ; i++)
				index[order[i]] = i;
			int offset = 0;
			for(unsigned int i=0 ; i<index.size() ; i++){
				memcpy(output + offset, bytes[index[i]], bSizes[index[i]]);
				offset += bSizes[index[i]];
				//delete bytes[index[i]];
			}

			return output;
		}

		int offset = 0;
		for(unsigned int i=0 ; i<bSizes.size() ; i++){
			memcpy(output + offset, bytes[i], bSizes[i]);
			offset += bSizes[i];
			//delete bytes[i];
		}

		return output;
	}
	static std::vector<unsigned int> datacmp_index(BYTE *data, unsigned int dataSize, BYTE *pattern, unsigned int patternSize, int max_count = 0)
	{
		std::vector<unsigned int> output;

		if(patternSize == 0)
			return std::vector<unsigned int>();
		if(dataSize < patternSize)
			return std::vector<unsigned int>();

		// �a�|�k
		for(unsigned int i=0 ; i<dataSize ; i++){
			bool isFound = false;
			if(data[i] == pattern[0]){
				isFound = true;
				for(unsigned int j=0 ; j<patternSize ; j++){
					if(data[i+j] != pattern[j]){
						isFound = false;
						break;
					}
				}
			}
			if(isFound){
				output.push_back(i);
				if(max_count <= 0)
					continue;
				else{
					if(output.size() == max_count)
						break;
				}
			}
		}

		return output;
	}
	static unsigned char* combineBytePointer(std::vector<unsigned char*> bytes, std::vector<unsigned int> bSizes, int &bufferSize)
	{
		int sumSize = 0;
		for(unsigned int i=0 ; i<bSizes.size() ; i++)
			sumSize += bSizes[i];
		bufferSize = sumSize;

		unsigned char *output = new unsigned char[sumSize];

		int offset = 0;
		for(unsigned int i=0 ; i<bSizes.size() ; i++){
			memcpy(output + offset, bytes[i], bSizes[i]);
			offset += bSizes[i];
			//delete bytes[i];
		}

		return output;
	}
	template <class T> static void Exchange(T &a, T &b){
			T change = a;
			a = b;
			b = change;
	}
	static void OutputDebugMessage(const char* str)
	{
		std::string tmp = "\n Error: *** ";
		tmp += str;
		tmp += "***\n\n";

		OutputDebugString(Util::stows(tmp.c_str()));
	}
};

#endif