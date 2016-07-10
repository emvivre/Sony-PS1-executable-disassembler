#include <ctype.h>
#include <stdio.h>

void hexdump(const void *e, int size)
{
	int i;
	char *p;
	
	p = (char *)e;
	for(i=0; i<size; i += 16, p += 16) { 
		int j;
		int nibble;
		char *addr;

		nibble = size - i;
		if ( nibble > 16 )
			nibble = 16;
		printf("%.8X  ", i);		
		addr = p;
		for (j=0; j<nibble; j++, addr++) {
			printf("%.2x ", (unsigned char) *addr);
			if (j == 7 || j == 15)
				printf(" ");
		}
		for (; j<16; j++) {
			printf("   ");
			if (j == 7 || j == 15)
				printf(" ");
		}
		addr = p;
		printf("|");
		for (j=0; j<nibble; j++, addr++) {
			char c;
			
			c = *addr;
			if (!isprint(c))
				c = '.';
			printf("%c", c);
		}
		printf("|\n");
	}
	printf("%08X\n", size);
}
