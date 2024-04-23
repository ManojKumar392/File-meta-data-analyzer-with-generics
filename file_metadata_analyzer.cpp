#include "file_metadata_analyzer.hpp"
#include <sys/stat.h>
#include <iostream>
#include <chrono> // For std::chrono
#include <ctime>  // For std::localtime
#include <fstream>
#include <sstream>
#include <algorithm>

// First encryption method structure
struct SimpleEncryption {
    std::string name = "SimpleEncryption";
    
    // Encryption method for a file
    std::string encrypt(const std::string& filename) {
        // Simple encryption logic (for demonstration purposes)
        std::string encrypted = "Encrypted with SimpleEncryption: " + filename;
        std::cout << encrypted << std::endl;
        return encrypted;
    }
};

// Second encryption method structure
struct AdvancedEncryption {
    std::string name = "AdvancedEncryption";
    
    // Encryption method for a file
    std::string encrypt(const std::string& filename) {
        // Advanced encryption logic (for demonstration purposes)
        std::string encrypted = "Encrypted with AdvancedEncryption: " + filename;
        std::cout << encrypted << std::endl;
        return encrypted;
    }
};


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
FileMetadataAnalyzer::analyze_numeric(const std::string& filename, T filesize) {
    // Retrieve metadata for the specified file
    FileMetadata metadata(filename);
    
    // Format file size as a string
    std::ostringstream oss;
    oss << filesize;
    
    // Print the file size
    std::cout << "File: " << filename << " has a size of " << oss.str() << " bytes." << std::endl;
    
    // Return the metadata object
    return metadata;
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


// Concept to check if the type has an encrypt method
template<typename T>
concept HasEncryptMethod = requires(T obj, const std::string& filename) {
    { obj.encrypt(filename) } -> std::convertible_to<std::string>;
};

// Updated FileAnalyzerHelper class
template<typename T>
class FileAnalyzerHelper {
public:
    // Method to access the private member of FileMetadataAnalyzer
    static void accessPrivateMember(FileMetadata& metadata) {

        // Access the private member 'isEncrypted' of FileMetadataAnalyzer
        if (metadata.isEncrypted) {
            std::cout << "The file is encrypted." << std::endl;
        } else {
            std::cout << "The file is not encrypted." << std::endl;

            // Using the template parameter T for encryption
            T encryptor;
            std::cout << "ENCRYPTING... the file of type " << metadata.extension() << " using " << encryptor.name << std::endl;
            encryptor.encrypt(metadata.filename()); // Perform encryption using the template parameter
            metadata.isEncrypted = true; // Mark file as encrypted
        }
    }
};

int calculateFileSize(const std::string& filename) {
    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return -1; // Return -1 on failure
    }

    // Get the current file position, which represents the file size
    int filesize = file.tellg();

    // Close the file
    file.close();

    return filesize;
}

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

    // Test numeric analysis using type traits (now focusing on file size)
    int filesize = calculateFileSize("file1.txt");
    FileMetadata numeric_metadata = FileMetadataAnalyzer::analyze_numeric("file1.txt", filesize);
    std::cout << "File metadata after numeric analysis: " << std::endl;
    std::cout << "Filename (Numeric): " << numeric_metadata.filename() << std::endl;

    // Test text analysis and word frequency container
    std::string text_filename = "sample_text.txt";
    //std::string text_filename = "sample_text2.txt";
    WordFrequencyContainer<std::string, int> wordFrequencies;
    FileMetadataAnalyzer::analyzeText(text_filename, wordFrequencies);
    std::cout << "\nWord Frequencies:" << std::endl;
    wordFrequencies.print();

    // Test template friendship
    FileMetadataAnalyzer fileAnalyzer;
    FileMetadata metadata = fileAnalyzer.analyze(filename1);

    FileMetadata metadata2 = fileAnalyzer.analyze(filename3);

    FileAnalyzerHelper<SimpleEncryption>::accessPrivateMember(metadata);

    FileAnalyzerHelper<AdvancedEncryption>::accessPrivateMember(metadata);
    
    FileAnalyzerHelper<SimpleEncryption>::accessPrivateMember(metadata2);

    

    return 0;
}
