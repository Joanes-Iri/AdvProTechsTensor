
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
		//shp = {};
		//vec = {};

		std::vector<int> tmp;
		tmp.assign({ 0 });
		vec = tmp;
	}

	// Constructs a tensor with arbitrary shape and zero-initializes all elements.
	Tensor(const std::vector< size_t >& shape) {
		this->shp = shape;
		int dataCount = 1;
		int shSize = shape.size();
		for (int i = 0; i < shSize; i++)
		{
			dataCount = dataCount * shape[i];
		}
		std::vector<int> tmp(dataCount, 0);
		vec = tmp;
	}

	// Constructs a tensor with arbitrary shape and fills it with the specified value.
	explicit Tensor(const std::vector< size_t >& shape, const ComponentType& fillValue)
	{
		this->shp = shape;
		int dataCount = 1;
		int shSize = shape.size();
		for (int i = 0; i < shSize; i++)
		{
			dataCount = dataCount * shape[i];
		}
		std::vector<int> tmp(dataCount);
		std::fill(tmp.begin(), tmp.end(), fillValue);
		vec = tmp;
	}

	// Copy-constructor.
	Tensor(const Tensor< ComponentType >& other) {

		std::copy(other.shape().begin(), other.shape().end(), std::back_inserter(this->shp));

		std::copy(other.data().begin(), other.data().end(), std::back_inserter(this->vec));

	}

	// Move-constructor.
	Tensor(Tensor< ComponentType >&& other) noexcept {

		this->shp = std::move(other.shape());
		this->vec = std::move(other.data());
	}

	// Copy-assignment
	Tensor&
		operator=(const Tensor< ComponentType >& other)
	{
		std::copy(other.shape().begin(), other.shape().end(), std::back_inserter(this->shp));
		std::copy(other.data().begin(), other.data().end(), std::back_inserter(this->vec));
		return *this;
	}

	// Move-assignment
	Tensor&
		operator=(Tensor< ComponentType >&& other) noexcept
	{
		this->shp = std::move(other.shape());
		this->vec = std::move(other.data());
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

	// Returns the data of the tensor in vector shape (how it is stored)
	[[nodiscard]] std::vector< int > data() const {
		return vec;
	}

	// Element access function
	const ComponentType&
		operator()(const std::vector< size_t >& idx) const
	{
		int linIndx = 0;
		int indSize = idx.size();
		int shSize = shape().size();
		for (int i = 0; i < indSize; i++)
		{
			if (i < (shSize - 1))
			{
				linIndx += this->shape()[i] * idx[i];
			}
			linIndx += idx[i];
		}
		return this->vec[linIndx];
	}

	// Element mutation function
	ComponentType&
		operator()(const std::vector< size_t >& idx) {
		int linIndx = 0;
		int indSize = idx.size();
		int shSize = shape().size();
		for (int i = 0; i < indSize; i++)
		{
			if (i < (shSize - 1))
			{
				linIndx += this->shape()[i] * idx[i];
			}
			linIndx += idx[i];
		}
		return this->vec[linIndx];
	}

private:
	std::vector< size_t > shp;
	std::vector< int > vec; // TODO: it cannot be only int. It needs to be able to hold float and other arithmetic types.
};

// Returns true if the shapes and all elements of both tensors are equal.
template< Arithmetic ComponentType >
bool operator==(const Tensor< ComponentType >& a, const Tensor< ComponentType >& b)
{
	bool eqTensors = false;
	if (std::equal(a.shape().begin(), a.shape().end(), b.shape().begin()))
	{
		if (std::equal(a.data().begin(), a.data().end(), b.data().begin()))
		{
			eqTensors = true;
		}
	}
	return eqTensors;

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
	// Temp variables
	int pos = 0;
	std::vector<size_t> shape;
	// Initialize local vector to store values temporarily
	std::vector< int > tempVec;

	// File read
	std::string valStr;
	std::ifstream inFile(filename);
	while (std::getline(inFile, valStr))
	{
		int rank = 0;
		int valInt = stoi(valStr);
		if (pos == 0)
		{
			rank = valInt;
		}
		else
		{
			if (pos < rank)
			{
				shape.push_back(valInt);
			}
			else
			{
				tempVec.insert(tempVec.end(), valInt);
			}
		}

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
	std::vector <size_t> shape = tensor.shape();
	int shSize = shape.size();
	for (int i = 0; i < shSize; i++) {
		textTensor.append(std::to_string(shape[i]) + "\n");
	}

	// Finally the Tensor values are written
	std::vector <int> data = tensor.data();
	int dataSize = tensor.numElements();
	for (int i = 0; i < dataSize; i++) {
		textTensor.append(std::to_string(data[i]) + "\n");
	}

	// TODO: write string to file
	std::fstream fileTens;
	fileTens.open(filename);
	if (!fileTens.is_open())
	{
		std::cout << "Unable to open the file: " + filename + "\n";
	}
	else
	{
		fileTens << textTensor;
		fileTens.close();
	}

}
