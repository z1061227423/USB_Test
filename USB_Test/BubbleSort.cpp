//ð�������㷨
void MyBubbleSort(int arr[], int len)
{
	int i, j, temp;
	int flag = 0, a = 1;
	int c = 3;
	for (i = 0; i < len - 1; i++)
	{
		for (j = 0; j < len - i - 1; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				temp = arr[j + 1];
				arr[j + 1] = arr[j];
				arr[j] = temp;
			}
		}
	}
}