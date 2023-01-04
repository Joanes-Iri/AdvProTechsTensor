
#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>


template<class T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<Arithmetic ComponentType>
class Tensor {
public:
    // Constructs a tensor with rank = 0 and zero-initializes the element.
    Tensor() {
        shp = {1};
        vec = {0};
    }

    // Constructs a tensor with arbitrary shape and zero-initializes all elements.
    Tensor(const std::vector<size_t> &shape) {
        this->shp = shape;
        int dataCount = 1;
        int shSize = shape.size();
        for (int i = 0; i < shSize; i++) {
            dataCount = dataCount * shape[i];
        }
        std::vector<int> tmp(dataCount, 0);
        std::move(tmp.begin(), tmp.end(), std::back_inserter(vec));
    }

    // Constructs a tensor with arbitrary shape and fills it with the specified value.
    explicit Tensor(const std::vector<size_t> &shape, const ComponentType &fillValue) {
        this->shp = shape;
        int dataCount = 1;
        int shSize = shape.size();
        for (int i = 0; i < shSize; i++) {
            dataCount = dataCount * shape[i];
        }
        std::vector<int> tmp(dataCount);
        std::fill(tmp.begin(), tmp.end(), fillValue);
        std::move(tmp.begin(), tmp.end(), std::back_inserter(vec));
    }

    // Copy-constructor.
    Tensor(const Tensor<ComponentType> &other) {
        this->shp = other.shape();
        this->vec = other.data();
    }

    // Move-constructor.
    Tensor(Tensor<ComponentType> &&other) noexcept {

        this->shp = std::move(other.shape());
        this->vec = std::move(other.data());
        other.emptyTensor();
    }

    // Copy-assignment
    Tensor &
    operator=(const Tensor<ComponentType> &other) {
        this->shp = other.shape();
        this->vec = other.data();
        return *this;
    }

    // Move-assignment
    Tensor &
    operator=(Tensor<ComponentType> &&other) noexcept {
        this->shp = std::move(other.shape());
        this->vec = std::move(other.data());
        other.emptyTensor();
        return *this;
    }

    // Destructor
    ~Tensor() {
        // clear shape vector
        shp.clear();
        // clear vec vector
        vec.clear();
    }

    // Returns the rank of the tensor.
    [[nodiscard]] size_t rank() const {
        return shp.size();
    }

    // Returns the shape of the tensor.
    [[nodiscard]] std::vector<size_t> shape() const {
        return shp;
    }

    // Returns the number of elements of this tensor.
    [[nodiscard]] size_t numElements() const {
        return vec.size();
    }

    // Returns the data of the tensor in vector shape (how it is stored)
    [[nodiscard]] std::vector<ComponentType> data() const {
        return vec;
    }

    // Element mutation function
    const ComponentType &
    operator()(const std::vector<size_t> &indices) const {
        size_t index = 0;
        for (size_t i = 0; i < indices.size(); i++) {
            index = index * shp[i] + indices[i];
        }
        return vec[index];
    }

    // Element access function
    ComponentType &
    operator()(const std::vector<size_t> &indices) {
        size_t index = 0;
        for (size_t i = 0; i < indices.size(); i++) {
            index = index * shp[i] + indices[i];
        }
        return vec[index];
    }

    void setData(const std::vector<ComponentType> &data) {
        vec = data;
    }

    void emptyTensor() {
        // clear shape vector
        shp.clear();
        // clear vec vector
        vec.clear();
        shp = {1};
        vec = {0};
    }

private:
    std::vector<size_t> shp;
    std::vector<ComponentType> vec;

};

// Returns true if the shapes and all elements of both tensors are equal.
template<Arithmetic ComponentType>
bool operator==(const Tensor<ComponentType> &a, const Tensor<ComponentType> &b) {
    if (a.rank() != b.rank()) return false;
    if (a.numElements() != b.numElements()) return false;

    for (size_t i = 0; i < a.rank(); i++) {
        if (a.shape()[i] != b.shape()[i]) return false;
    }

    for (size_t i = 0; i < a.numElements(); i++) {
        if (a.data()[i] != b.data()[i]) return false;
    }

    return true;
}

template<Arithmetic ComponentType>
int printTensor(std::ostream &out, std::vector<size_t> shape, std::vector<ComponentType> data, int idx,
            size_t rank, bool last = false) {

    out << "[";
    // Loop through the outermost dimension in the current recursive iteration
    for (size_t i = 0; i < shape[0]; ++i) {
        // Check if it's not the innermost dimension
        if (shape.size() > 1) {
            bool newLast = false;
            // If loop is about to finish then we want to print ']' instead of '], ' when recursive iteration finishes
            if (i == (shape[0] - 1)) {
                // If outermost rank then print ']' instead of '], '
                if (shape.size() == rank)
                    last = true;
                newLast = true;
            }
            std::vector<size_t> newShape;
            newShape.insert(newShape.cbegin(), shape.begin() + 1, shape.end());
            // Recursive call for the next innermost dimension
            idx = printTensor(out, newShape, data, idx, rank, newLast);
        } else {
            // Innermost dimension --> print out
            out << data[idx];
            ++idx;
            if (i < (shape[0] - 1))
                out << " ";
        }
    }
    if (!last)
        out << "]," << std::endl;

    else
        out << "]";

    return idx;
}

// Pretty-prints the tensor to stdout.
// This is not necessary (and not covered by the tests) but nice to have, also for debugging (and for exercise of course...).
template<Arithmetic ComponentType>
std::ostream &
operator<<(std::ostream &out, const Tensor<ComponentType> &tensor) {
    // Get the shape and data of the tensor
    std::vector<size_t> shape = tensor.shape();
    std::vector<ComponentType> data = tensor.data();

    // If the tensor has rank 0, just output the single element
    if (shape.empty()) {
        out << data[0];
        return out;
    }

    // If the tensor has rank 1, output the elements as a list
    if (shape.size() == 1) {
        out << "[";
        for (size_t i = 0; i < data.size(); i++) {
            out << data[i];
            if (i < data.size() - 1) {
                out << ", ";
            }
        }
        out << "]";
        return out;
    }

    printTensor(out, shape, data, 0, tensor.rank());
    return out;
}

// Reads a tensor from file.
template<Arithmetic ComponentType>
Tensor<ComponentType> readTensorFromFile(const std::string &filename) {
    // Temp variables
    int pos = 0;
    int rank = 0;
    std::vector<size_t> shape;
    // Initialize local vector to store values temporarily
    std::vector<ComponentType> tempVec;

    // Read numeric value
    std::string valStr;
    // File read
    std::ifstream inFile(filename);
    while (std::getline(inFile, valStr)) {
        // Convert string to numeric type
        ComponentType val = stoi(valStr);
        if (pos == 0) {
            rank = val;
        } else {
            if (pos <= rank) {
                shape.push_back(val);
            } else {
                tempVec.insert(tempVec.end(), val);
            }
        }

        pos++;
    }

    // Construct tensor from vector
    Tensor<ComponentType> newTensor(shape);
    newTensor.setData(tempVec);

    return newTensor;
}

// Writes a tensor to file.
template<Arithmetic ComponentType>
void writeTensorToFile(const Tensor<ComponentType> &tensor, const std::string &filename) {
    int rank = tensor.rank();
    // text to write on file
    std::string textTensor;
    // First rank is written
    textTensor.append(std::to_string(rank) + "\n");

    // Next the shape is written
    std::vector<size_t> shape = tensor.shape();
    int shSize = shape.size();
    for (int i = 0; i < shSize; i++) {
        textTensor.append(std::to_string(shape[i]) + "\n");
    }

    // Finally the Tensor values are written
    std::vector<ComponentType> data = tensor.data();
    int dataSize = tensor.numElements();
    for (int i = 0; i < dataSize; i++) {
        textTensor.append(std::to_string(data[i]) + "\n");
    }

    std::ofstream fileTens;
    fileTens.open(filename);
    if (!fileTens.is_open()) {
        std::cout << "Unable to open the file: " + filename + "\n";
    } else {
        fileTens << textTensor;
        fileTens.close();
    }

}

