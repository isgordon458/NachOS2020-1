#include "syscall.h"
main()
{
	int	n;

	static char arr[2048];
	int b[1000];
	int a=0;
arr[655] = 1;
a = arr[655];
PrintInt(a);
arr[365] = 2;
a = arr[365];
PrintInt(a);
arr[673] = 3;
a = arr[673];
PrintInt(a);
arr[782] = 4;
a = arr[782];
PrintInt(a);
arr[344] = 5;
a = arr[344];
PrintInt(a);
arr[183] = 6;
a = arr[183];
PrintInt(a);
arr[278] = 7;
a = arr[278];
PrintInt(a);
arr[241] = 8;
a = arr[241];
PrintInt(a);
arr[298] = 9;
a = arr[298];
PrintInt(a);
arr[279] = 10;
a = arr[279];
PrintInt(a);
arr[954] = 11;
a = arr[954];
PrintInt(a);
arr[2] = 12;
a = arr[2];
PrintInt(a);
arr[68] = 13;
a = arr[68];
PrintInt(a);
arr[796] = 14;
a = arr[796];
PrintInt(a);
arr[743] = 15;
a = arr[743];
PrintInt(a);
for (n = 50; n > 5; n--)
	PrintInt(n);
for (n = 1000; n > 5; n--)
	a = n;
arr[655] = 1;
a = arr[655];
PrintInt(a);
arr[365] = 2;
a = arr[365];
PrintInt(a);
arr[673] = 3;
a = arr[673];
PrintInt(a);
arr[782] = 4;
a = arr[782];
PrintInt(a);
arr[344] = 5;
a = arr[344];
PrintInt(a);

}
