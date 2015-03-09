#include<iostream>
#include<thread>
#include<algorithm>
#include<numeric>
#include<vector>

using namespace std;

template<typename Iterator, typename T>
struct accumulate_block{
	void operator() (Iterator first, Iterator last, T & result){
		result = accumulate(first, last, result);
	}
};

template<typename Iterator, typename T>
T parallel_accmulate(Iterator first, Iterator last, T init){
	unsigned long const length = distance(first, last);
	if(!length)
		return init;
	unsigned long const hardware_threads = thread::hardware_concurrency();
	unsigned long const num_threads = hardware_threads;
	cout<<"number of threads: "<<num_threads<<endl;

	unsigned long const block_size = length/num_threads;

	vector<T> results(num_threads);
	vector<thread> threads(num_threads-1);

	Iterator block_start = first;
	for(unsigned long i = 0; i < num_threads - 1; i++){
		Iterator block_end = block_start;
		advance(block_end, block_size);
		threads[i] = thread(accumulate_block<Iterator, T>(), block_start, block_end, ref(results[i]));
		block_start = block_end;
	}

	accumulate_block<Iterator, T>()(block_start, last, results[num_threads-1]);
	
	for_each(threads.begin(), threads.end(), mem_fn(&thread::join));

	return accumulate(results.begin(), results.end(), init);
}

int main(){
	vector<int> numbers(100,1);
	cout<<parallel_accmulate(numbers.begin(), numbers.end(), 1)<<endl;
	return 1;
}
