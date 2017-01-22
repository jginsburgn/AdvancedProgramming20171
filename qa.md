#Questions and Answers
### How to differentiate between `C` and `C++` libraries?
### What are `.so`, `.a` and `.o` files produced by `GCC`?
### How do you make your own `.so` file?
### How do you make your own `.a` file?
### How do you make your own `.o` file?
### What is the difference between automatic, dynamic and static memory allocation?
### In `C`, returning an automatically allocated variable will produce a zombie or a memory leak, or is it illegal?

```C
int foo() {
	int a = 15;
	return a;
}

int main () {
	int b = foo(); //**here**
	return 0;
}
```
And, how does it relate to the [Return Value Optimization](https://en.wikipedia.org/wiki/Return_value_optimization)?

### How do structs get copied in C?

```C
struct Account {
   int account_number;
   char *first_name;
   char *last_name;
   float balance;
};

int main () {
	Account acct;
	Account cpy = acct;
	return 0;
}
```