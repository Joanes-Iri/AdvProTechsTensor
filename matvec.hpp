#pragma once

#include "tensor.hpp"

template<typename ComponentType>
class Vector {
public:
    // Default-constructor.
    Vector() = default;

    // Constructor for vector of certain size.
    explicit Vector(size_t size) {
        std::vector < size_t > shape = {size};
        tensor_ = Tensor< ComponentType >(shape);
    }

    // Constructor for vector of certain size with constant fill-value.
    Vector(size_t size, const ComponentType &fillValue) {
        tensor_(size, fillValue);
    }

    // Constructing vector from file.
    Vector(const std::string &filename) {
        tensor_ = readTensorFromFile<ComponentType>(filename);
    }

    // Number of elements in this vector.
    [[nodiscard]] size_t size() const {
        return tensor_.numElements();
    }

    // Element access function
    const ComponentType &
    operator()(size_t idx) const {
        std::vector<size_t> indices = {idx};
        return tensor_(indices);
    }

    // Element mutation function
    ComponentType &
    operator()(size_t idx) {
        std::vector<size_t> indices = {idx};
        return tensor_(indices);
    }

    // Reference to internal tensor.
    Tensor<ComponentType> &tensor() {
        return tensor_;
    }

private:
    Tensor<ComponentType> tensor_;
};

template<typename ComponentType>
class Matrix {
public:
    // Default-constructor.
    Matrix() = default;

    // Constructor for matrix of certain size.
    explicit Matrix(size_t rows, size_t cols) {
        rws = rows;
        cls = cols;
        std::vector<size_t> shape = {rows, cols};
        tensor_(shape);
    }

    // Constructor for matrix of certain size with constant fill-value.
    Matrix(size_t rows, size_t cols, const ComponentType &fillValue) {
        rws = rows;
        cls = cols;
        std::vector<size_t> shape = {rows, cols};
        tensor_(shape, fillValue);
    }

    // Constructing matrix from file.
    Matrix(const std::string &filename) {
        tensor_ = readTensorFromFile<ComponentType>(filename);
        rws = tensor_.shape()[0];
        cls = tensor_.shape()[1];
    }

    // Number of rows.
    [[nodiscard]] size_t rows() const {
        return rws;
    }

    // Number of columns.
    [[nodiscard]] size_t cols() const {
        return cls;
    }

    // Element access function
    const ComponentType &
    operator()(size_t row, size_t col) const {
        std::vector<size_t> indx = {row, col};
        return tensor_(indx);
    }

    // Element mutation function
    ComponentType &
    operator()(size_t row, size_t col) {
        std::vector<size_t> indx = {row, col};
        return tensor_(indx);
    }

    // Reference to internal tensor.
    Tensor<ComponentType> &tensor() {
        return tensor_;
    }

private:
    int rws;
    int cls;
    Tensor<ComponentType> tensor_;
};



// Performs a matrix-vector multiplication.
template<typename ComponentType>
Vector<ComponentType> matvec(const Matrix<ComponentType> &mat, const Vector<ComponentType> &vec) {
    if (mat.cols() != vec.size()) {
        throw std::invalid_argument("matvec: matrix and vector sizes do not match");
    }
    Vector<ComponentType> result(mat.rows());
    for (size_t i = 0; i < mat.rows(); ++i) {
        for (size_t j = 0; j < mat.cols(); ++j) {
            result(i) += mat(i, j) * vec(j);
        }
    }
    return result;
}

