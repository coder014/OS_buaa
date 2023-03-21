#include <blib.h>

size_t strlen(const char *s) {
	size_t slen = 0;
	while (*s++)
		++slen;
	return slen;
}

char *strcpy(char *dst, const char *src) {
	char *res = dst;
	while(*dst++ = *src++);
	return res;
}

char *strncpy(char *dst, const char *src, size_t n) {
	char *res = dst;
	while (*src && n--) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strcat(char *dst, const char *src) {
	char *res = dst;
	dst += strlen(dst);
	strcpy(dst, src);
	return res;
}

int strcmp(const char *s1, const char *s2) {
	char c1, c2;
	do {
		c1 = *s1++;
		c2 = *s2++;
		if (c1 == '\0')
			return c1-c2;
	} while (c1 == c2);
	return c1-c2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n--) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		if (*s1 == 0) {
			break;
		}
		s1++;
		s2++;
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	void *res = s;
	for (size_t i=0; i<n; i++) {
		*((unsigned char *)s++) = (unsigned char)c;
	}
	return res;
}

void *memcpy(void *out, const void *in, size_t n) {
	char *csrc = (char *)in;
	char *cdest = (char *)out;
	for (int i = 0; i < n; i++) {
		cdest[i] = csrc[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	char *c1 = (char *)s1;
	char *c2 = (char *)s2;
	for (size_t i=0; i<n; i++) {
		if(c1[i] != c2[i])
			return c1[i] - c2[i];
	}
	return 0;
}
