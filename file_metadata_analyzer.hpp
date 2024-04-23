#ifndef FILE_METADATA_ANALYZER_HPP
#define FILE_METADATA_ANALYZER_HPP

#include <string>
#include <vector>
#include <type_traits> // For type traits
#include <iostream> // For fold expression

// Class representing file metadata
class FileMetadata {
public:
    FileMetadata(const std::string& filename);

    std::string filename() const;
    std::string extension() const;
    std::string last_modified() const;
    bool is_numeric() const;

    // Template friendship example
    template<typename T>
    friend class FileAnalyzerHelper;

private:
    std::string m_filename;
    std::string m_extension;
    std::string m_last_modified;
    bool m_is_numeric;
    bool isEncrypted = false;
};

// Custom container class to store word frequencies
template<typename Key, typename Value>
class WordFrequencyContainer {
private:
    struct Node {
        Key key;
        Value value;
        Node* next;
        Node(const Key& k, const Value& v) : key(k), value(v), next(nullptr) {}
    };

    Node* head;

public:
    WordFrequencyContainer() : head(nullptr) {}

    ~WordFrequencyContainer() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Operator+ overload to increment word frequency
    WordFrequencyContainer<Key, Value>& operator+(const Key& key) {
        Node* current = head;
        while (current != nullptr) {
            if (current->key == key) {
                current->value++;
                return *this;
            }
            current = current->next;
        }
        // If key not found, add new node
        Node* newNode = new Node(key, 1);
        newNode->next = head;
        head = newNode;
        return *this;
    }

    // Print function to display word frequencies
    void print() const {
        Node* current = head;
        while (current != nullptr) {
            std::cout << current->key << ": " << current->value << std::endl;
            current = current->next;
        }
    }
};

// Class for analyzing file metadata
class FileMetadataAnalyzer {
public:
    // Analyze metadata for a single file
    static FileMetadata analyze(const std::string& filename);

    // Analyze metadata for multiple files using variadic templates
    template<typename... Args>
    static std::vector<FileMetadata> analyze(const Args&... filenames);

    // Template specialization for analyze function
    template<typename T>
    static std::vector<FileMetadata> analyze(const T& filename);

    // Lambda template for analyzing file metadata
    template<typename Func>
    static void analyze_lambda(const std::string& filename, Func&& func);

    // concept : Type traits
    template<typename T>
    static typename std::enable_if<std::is_arithmetic<T>::value, FileMetadata>::type
    analyze_numeric(const std::string& filename, T filesize);

    // Analyze text and store word frequencies
    template<typename Container>
    static void analyzeText(const std::string& filename, Container& wordFrequencies);
};

#endif // FILE_METADATA_ANALYZER_HPP

