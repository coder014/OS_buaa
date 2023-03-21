#include <stdio.h>
int s[8], len;
int main()
{
	int n, flag=1;
	scanf("%d", &n);
	while (n)
	{
		s[len++]=n%10;
		n/=10;
	}
	for (int i=0;i<len+1>>1;i++)
		if(s[i]!=s[len-i-1])
			flag=0;
	if (flag)
		printf("Y\n");
	else
		printf("N\n");
	return 0;
}
