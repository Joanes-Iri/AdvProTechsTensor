
#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>


template< class T >
concept Arithmetic = std::is_arithmetic_v< T >;

template< Arithmetic ComponentType >
class Tensor
{
public:
	// Constructs a tensor with rank = 0 and zero-initializes the element.
	Tensor()
	{
		shp = {};
		vec = {};
	}

	// Constructs a tensor with arbitrary shape and zero-initializes all elements.
	Tensor(const std::vector< size_t >& shape) { // TODO: HOW DO YOU INITIALIZE A VECTOR TO BE OF A UNKNOWN SHAPE / RANK?
		this->shp = shape;
		int endPos = shape.size();
		std::vector<int> zeros(endPos, 0);
		if (std::size_t i = 1; i != shape.size())
		{
			int size = shape.at(i);
			dims.push_back(dims);
			i++;
		}
	}

	// Constructs a tensor with arbitrary shape and fills it with the specified value.
	explicit Tensor(const std::vector< size_t >& shape, const ComponentType& fillValue)
	{
		this->shp = shape;
		std::vector< std::vector< size_t > > vec1(this->shp[0], std::vector<int>(this->shp[1], 0));
		vec = vec1;
		std::fill(vec.begin(), vec.end(), fillValue);
	}

	// Copy-constructor.
	Tensor(const Tensor< ComponentType >& other) {

		std::vector< size_t > copShape;
		std::copy(other.shp.begin(), other.shp.end(), copShape);

		std::vector< std::vector< size_t > > copContents;
		std::copy(other.vec.begin(), other.vec.end(), copContents);

		Tensor<ComponentType> returnTensor(copShape, copContents);
	}

	// Move-constructor.
	Tensor(Tensor< ComponentType >&& other) noexcept {

		std::vector< size_t > copShape = std::move(other->shape);
		std::vector< std::vector< size_t > > copContents = std::move(other->vec);

		Tensor<ComponentType> returnTensor(copShape, copContents); // TODO: might be wrong
	}

	// Copy-assignment
	Tensor&
		operator=(const Tensor< ComponentType >& other);

	// Move-assignment
	Tensor&
		operator=(Tensor< ComponentType >&& other) noexcept;

	// Destructor
	~Tensor() {
		// clear shape vector
		shp.clear();
		// clear vec vector
		vec.clear();
	}

	// Returns the rank of the tensor.
	[[nodiscard]] size_t rank() const
	{
		return std::rank<decltype(vec)>::value;
	}

	// Returns the shape of the tensor.
	[[nodiscard]] std::vector< size_t > shape() const
	{
		return shp; // TODO: it's supposed to be a tuple of twp
	}

	// Returns the number of elements of this tensor.
	[[nodiscard]] size_t numElements() const {
		return vec.size();
	}

	// Element access function
	const ComponentType&
		operator()(const std::vector< size_t >& idx) const;

	// Element mutation function
	ComponentType&
		operator()(const std::vector< size_t >& idx);

private:
	std::vector< size_t > shp;
	std::vector<int> vec;
};

// TODO: Implement all methods of the Tensor class template.

// Returns true if the shapes and all elements of both tensors are equal.
template< Arithmetic ComponentType >
bool operator==(const Tensor< ComponentType >& a, const Tensor< ComponentType >& b)
{
	return std::equal(a.shape().begin(), a.shape().end(), b.shape().begin());

}

// Pretty-prints the tensor to stdout.
// This is not necessary (and not covered by the tests) but nice to have, also for debugging (and for exercise of course...).
template< Arithmetic ComponentType >
std::ostream&
operator<<(std::ostream& out, const Tensor< ComponentType >& tensor)
{
	return out << tensor;
}

// Reads a tensor from file.
template< Arithmetic ComponentType >
Tensor< ComponentType > readTensorFromFile(const std::string& filename)
{
	// Initialize local vector to store values temporarily
	std::vector< int > tempVec = {};

	// File read
	std::string valStr;
	std::ifstream inFile(filename);
	while (std::getline(inFile, valStr))
	{
		int valInt = stoi(valStr);
		tempVec.insert(tempVec.end(), valInt);
	}

	// Construct tensor from vector
	Tensor < int > newTensor({ tempVec.begin(), tempVec.end() });
	return newTensor;
}

// Writes a tensor to file.
template< Arithmetic ComponentType >
void writeTensorToFile(const Tensor< ComponentType >& tensor, const std::string& filename)
{
	// TODO: Implement this function to write tensors to file.
	//       The format is defined in the instructions.
	int rank = tensor.rank();
	// text to write on file
	std::string textTensor;
	// First rank is written
	textTensor.append(std::to_string(rank) + "\n");

	// Next the shape is written
	std::vector <int> shape = tensor.shape();
	for (int i = 0; i < tensor.rank(); i++) {
		textTensor.append(std::to_string(shape[i]) + "\n");
	}

}