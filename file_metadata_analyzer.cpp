#include "file_metadata_analyzer.hpp"
#include <sys/stat.h>
#include <ctime>
#include <iostream>
#include <chrono> // For std::chrono
#include <ctime>  // For std::localtime
#include <fstream>
#include <sstream>
#include <algorithm>

FileMetadata::FileMetadata(const std::string& filename) : m_filename(filename) {
    // Initialize extension
    size_t pos = filename.find_last_of('.');
    m_extension = (pos != std::string::npos) ? filename.substr(pos + 1) : "";

    // Initialize last modified time
    struct stat result;
    if (stat(filename.c_str(), &result) == 0) {
        std::time_t modified_time = result.st_mtime;
        std::tm* modified_tm = std::localtime(&modified_time);
        char buffer[80]; // Buffer for formatted time string
        std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", modified_tm);
        m_last_modified = buffer;
    } else {
        m_last_modified = "N/A"; // Default value
    }
}

std::string FileMetadata::filename() const {
    return m_filename;
}

std::string FileMetadata::extension() const {
    // Extract file extension from filename
    size_t pos = m_filename.find_last_of('.');
    return (pos != std::string::npos) ? m_filename.substr(pos + 1) : "";
}

std::string FileMetadata::last_modified() const {
    // Get last modified time of file
    struct stat result;
    if (stat(m_filename.c_str(), &result) == 0) {
        // Convert time to a string format (replace with your desired format)
        std::time_t modified_time = result.st_mtime;
        return std::asctime(std::localtime(&modified_time));
    }
    // Throw an exception when the file is not found
    throw std::runtime_error("File not found: " + m_filename);
}

bool FileMetadata::is_numeric() const {
    return m_is_numeric;
}

FileMetadata FileMetadataAnalyzer::analyze(const std::string& filename) {
    return FileMetadata(filename);
}

template<typename... Args>
std::vector<FileMetadata> FileMetadataAnalyzer::analyze(const Args&... filenames) {
    std::vector<FileMetadata> metadata;
    (metadata.push_back(FileMetadata(filenames)), ...); // Concept: Fold Expression to analyze multiple files (5)
    return metadata;
}

template<>
std::vector<FileMetadata> FileMetadataAnalyzer::analyze<>(const std::string& filename) {
    std::vector<FileMetadata> metadata;
    metadata.push_back(FileMetadata(filename));
    return metadata;
}

template<typename Func>
void FileMetadataAnalyzer::analyze_lambda(const std::string& filename, Func&& func) {
    func(FileMetadata(filename)); // Execute lambda function with file metadata
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, FileMetadata>::type
FileMetadataAnalyzer::analyze_numeric(T value) {
    // Example of using type traits to analyze numeric data
    return FileMetadata(std::to_string(value));
}

template<typename Container>
void FileMetadataAnalyzer::analyzeText(const std::string& filename, Container& wordFrequencies) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("File not found: " + filename);
    }
    std::string word;
    while (file >> word) {
        // Convert word to lowercase for case-insensitive analysis
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
        // Remove non-alphabetic characters (punctuation, etc.)
        word.erase(std::remove_if(word.begin(), word.end(), [](unsigned char c) { return !std::isalpha(c); }), word.end());
        if (!word.empty()) {
            // Increment word frequency in the container using operator+
            wordFrequencies + word;
        }
    }
}

// Helper class to demonstrate template friendship
template<typename T>
class FileAnalyzerHelper { 
//This class may contain methods to help the analyzer in its tasks , Template friendship would help in that regards    
public:
    static void accessPrivateMember() {
        FileMetadataAnalyzer File_friend;
        if(File_friend.isEncrypted) {
            std::cout<<std::endl<<"The file is encrypted";
        }
        else {
            std::cout<<std::endl<<"The file is not encrypted \n ENCRYPTING...";
            File_friend.isEncrypted = true;
        }
    }
};

int main() {
    // Test single file analysis
    std::string filename1 = "file1.txt";
    FileMetadata metadata1 = FileMetadataAnalyzer::analyze(filename1);
    std::cout << "Filename: " << metadata1.filename() << std::endl;
    std::cout << "Extension: " << metadata1.extension() << std::endl;
    std::cout << "Last Modified: " << metadata1.last_modified() << std::endl;

    // Test multiple files analysis using variadic templates
    std::string filename2 = "file2.txt";
    std::string filename3 = "file3.pdf";
    std::vector<FileMetadata> metadata_multiple = FileMetadataAnalyzer::analyze(filename2, filename3);
    for (const auto& metadata : metadata_multiple) {
        std::cout << "Filename: " << metadata.filename() << std::endl;
        std::cout << "Extension: " << metadata.extension() << std::endl;
        std::cout << "Last Modified: " << metadata.last_modified() << std::endl;
    }

    // Test lambda analysis
    std::string filename4 = "file3.pdf";
    FileMetadataAnalyzer::analyze_lambda(filename4, [](const FileMetadata& metadata) {
        std::cout << "Filename (Lambda): " << metadata.filename() << std::endl;
        std::cout << "Extension (Lambda): " << metadata.extension() << std::endl;
        std::cout << "Last Modified (Lambda): " << metadata.last_modified() << std::endl;
    });

    // Test numeric analysis using type traits
    int numeric_value = 12345;
    FileMetadata numeric_metadata = FileMetadataAnalyzer::analyze_numeric(numeric_value);
    std::cout << "Filename (Numeric): " << numeric_metadata.filename() << std::endl;

    // Test text analysis and word frequency container
    std::string text_filename = "sample_text.txt";
    //std::string text_filename = "sample_text2.txt";
    WordFrequencyContainer<std::string, int> wordFrequencies;
    FileMetadataAnalyzer::analyzeText(text_filename, wordFrequencies);
    std::cout << std::endl<< "Word Frequencies:" << std::endl;
    wordFrequencies.print();

    // Test template friendship
    FileAnalyzerHelper<int>::accessPrivateMember();

    return 0;
}
