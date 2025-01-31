Solutions Considered
I looked into a few different ways to handle log retrieval:

1. Single-threaded Processing
This would process the log file one line at a time. It's simple, but it would be too slow for large log files since it only uses one CPU core.

2. Multi-threaded Processing
This approach splits the file into chunks and processes them in parallel, using multiple CPU cores. It’s much faster and more scalable, but it introduces challenges with thread safety when writing to the same output file.

3. Buffered Reading
We also thought about reading the file in big chunks using buffers. However, this approach still had the same thread safety issues as the multi-threaded approach, so we focused on that instead.

4. Binary Search Approach
I initially considered using a binary search to quickly find log entries starting with the given date. However, I found out that the log entries are in random order, which made this approach unfeasible.

Final Solution Summary
The final solution uses multi-threading to divide the log file into chunks and process them in parallel. This is the best choice because:

Speed: Multi-threading speeds up processing by using multiple CPU cores.
Scalability: It handles large files well and scales as needed.
Thread Safety: A mutex ensures that only one thread writes to the output file at a time, avoiding data corruption.
Logs are saved in a file named output/output_YYYY-MM-DD.txt, and if the output directory doesn't exist, it's created automatically.

Steps to Run
Make sure your compiler supports C++17 (required for filesystem). Use -std=c++17 flag while compiling.

Example:
Run the program by passing the date as an argument (in YYYY-MM-DD format):
./extract_logs 2024-01-31
The program will create the output folder if it doesn't exist and save the filtered logs to output/output_2024-01-31.txt.

