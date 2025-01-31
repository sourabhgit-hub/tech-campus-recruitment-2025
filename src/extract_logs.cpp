#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

const int NUM_THREADS = thread::hardware_concurrency();  // Get number of CPU cores
mutex print_mutex;  // Mutex for synchronized output

// Function to process a chunk of the file
void processChunk(const string &filename, streampos start, streampos end, const string &date, ofstream &outputFile) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    file.seekg(start);  // Move to the start position

    string line;
    while (file.tellg() < end && getline(file, line)) {
        if (line.find(date) == 0) {  // Check if the log starts with the date
            lock_guard<mutex> lock(print_mutex);
            outputFile << line << endl;  // Write the line to the output file
        }
    }

    file.close();
}

// Function to get file size
streampos getFileSize(const string &filename) {
    ifstream file(filename, ios::ate | ios::binary);
    return file.tellg();
}

// Function to divide file processing across multiple threads
void retrieveLogsParallel(const string &filename, const string &date, const string &outputFilename) {
    streampos fileSize = getFileSize(filename);
    streampos chunkSize = fileSize / NUM_THREADS;

    // Open the output file in write mode. It will create the file if it doesn't exist.
    ofstream outputFile(outputFilename, ios::trunc);  // Open output file for writing, truncating if exists
    if (!outputFile) {
        cerr << "Error opening output file: " << outputFilename << endl;
        return;
    }

    vector<thread> threads;

    for (int i = 0; i < NUM_THREADS; i++) {
        streampos start = i * chunkSize;
        streampos end = (i == NUM_THREADS - 1) ? fileSize : start + chunkSize;

        threads.emplace_back(processChunk, filename, start, end, date, ref(outputFile));
    }

    for (auto &t : threads) {
        t.join();  // Wait for all threads to finish
    }

    outputFile.close();  // Close the output file
}

int main(int argc, char* argv[]) {
    // Check if the date argument is passed
    if (argc != 2) {
        cout << "Date argument is not passed\n";
        return 1;
    }
    string date = argv[1];

    // Validate date format
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        cout << "Error: Invalid date format. Please use YYYY-MM-DD." << endl;
        return 1;
    }


    // Ensure that the output directory exists
    namespace fs = std::filesystem;
    
    fs::path current_path = fs::current_path();  // Get the current working directory
    fs::path parent_path = current_path.parent_path();  // Get the parent directory
    fs::path output_dir = parent_path / "output"; // Combine with the folder name
    if (!fs::exists(output_dir)) {
        fs::create_directory(output_dir);  // Create the directory if it doesn't exist
    }
    string outputFilename = output_dir;
    outputFilename+=( "/output_" + date + ".txt");
    cout<<outputFilename<<endl;
    // return 0;
    // Call the log extraction function
    string filename = "logs_2024.log";
    retrieveLogsParallel(filename, date, outputFilename);

    cout << "Logs saved to " << outputFilename << endl;

    return 0;
}
