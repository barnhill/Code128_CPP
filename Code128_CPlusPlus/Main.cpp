#include "C128.h"
#include <ctime>
using namespace std;

int main(int argc, char **argv)
{ 
	if (argc != 4) 
	{
		printf("Usage: %s barWidthInPx heightInPx filename\n", argv[0]);
		return 1;
	}

	printf("Code 128 Test Util\n");
	printf("==================\n");

	Code128 code128("038000356216", DYNAMIC);
	printf("Barcode object created\n");

	code128.writeImage(atoi(argv[1]), atoi(argv[2]), argv[3]);
	printf("File write complete.\n");
}