uint32_t clear_1(uint32_t input, int arr[], int n)
{
  for (int i = 0; i < n; ++i)
		input = input & (~(1 << arr[i]));
	return input;
}

uint32_t set_1(uint32_t input, int arr[], int n)
{
  for (int i = 0; i < n; ++i)
		input = input | (1 << arr[i]);
	return input;
}

uint32_t and(uint32_t input, int arr[], int n)
{
	for (int i = 0; i < n; ++i)
		input = input & (~(3 << (arr[i]*2)));
	return input;
}

uint32_t or(uint32_t input, int arr[], int n, uint32_t mode)
{
	for (int i = 0; i < n; ++i)
		input = input | (mode << (arr[i]*2));
	return input;
}

uint32_t set(uint32_t input, int arr[], int n, uint32_t mode)
{
	for (int i = 0; i < n; ++i)
		input = input & (~(3 << (arr[i]*2)));
	for (int i = 0; i < n; ++i)
		input = input | (mode << (arr[i]*2));
	return input;
}