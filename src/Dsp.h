#pragma once
#include <map>
#include <numeric>

#include "Signal.h"



/// @brief Convenience functions and constants for digital signal processing
namespace dsp
{
	// Constants
	const double pi = 3.14159265358979311600;

	/// @brief Return evenly spaced values within a given interval.
	///
	/// Values are generated within the half-open interval [start, stop) (in other
	/// words, the interval including start but excluding stop). 
	/// @tparam T Type of the elements in vector.
	/// @param start Start of interval. The interval includes this value.
	/// @param stop End of interval. The interval does not include this value.
	/// @param step Spacing between values. For any output out, this is the distance
	/// between two adjacent values, out[i+1] - out[i]. The default step size is 1. 
	/// @return Vector of evenly spaced values
	template<class T>
	std::vector<T> arange(T start, T stop, T step = 1)
	{
		std::vector<T> values;
		for (T value = start; value < stop; value += step)
		{
			values.push_back(value);
		}
		return values;
	}

	/// @brief Returns the energy of the passed range.
	/// @tparam T Type of the elements in range.
	/// @param start Iterator pointing to the start of the range (e.g. my_signal.begin())
	/// @param end Iterator pointing to the end of the range (e.g. my_signal.end())
	/// @return The energy of the range (sum of the squared samples in range)
	template<class T>
	T calculateEnergy(typename std::vector<T>::iterator start, 
		typename std::vector<T>::iterator end);

	/// @brief Returns the mean power of the passed range
	/// @tparam T Type of the elements in range.
	/// @param start Iterator pointing to the start of the range (e.g. my_signal.begin())
	/// @param end Iterator pointing to the end of the range (e.g. my_signal.end())
	/// @return The mean power of the range (energy divided by length)
	template<class T>
	T calculateMeanPower(typename std::vector<T>::iterator start,
		typename std::vector<T>::iterator end);
	
	/// @brief Returns the center portion of a vector
	/// @tparam T Type of the elements in vector.
	/// @param vec Input vector
	/// @param newSize Length of the extracted, centered portion
	/// @return Centered portion of the vector
	template<class T>
	std::vector<T> centered(const std::vector<T>& vec, size_t newSize);

	
	/// @brief Join a sequence of vectors.
	/// @tparam T Type of the vector elements
	/// @param vectors Vector containing references to the vectors.
	/// @return The concatenated vector. 
	template<class T>
	std::vector<T> concatenate(std::vector<std::vector<T>*> vectors)
	{
		std::vector<T> concatenated_vector;
		for (const auto& v : vectors)
		{
			concatenated_vector.insert(concatenated_vector.end(), v->begin(), v->end());
		}

		return concatenated_vector;
	}


	/* Convolutions and correlations */

	enum class convolution_mode { full, valid, same };
	enum class convolution_method { automatic, direct, fft };
	

	/// @brief Find the fastest convolution/correlation method.
	/// This primarily exists to be called during the method = 'auto' option in
	/// convolve and correlate. It can also be used to determine the value of
	/// method for many different convolutions of the same length. In addition,
	/// it supports timing the convolution to adapt the value of method to a
	/// particular set of inputs and/or hardware.
	/// @tparam T Type of the vector elements
	/// @param in1 The first argument passed into the convolution function.
	/// @param in2 The second argument passed into the convolution function.
	/// @param mode An enum indicating the size of the output:
	/// full:	The output is the full discrete linear cross-correlation of the inputs. (Default)
	///	valid: 	The output consists only of those elements that do not rely on the zero-padding. In �valid� mode, either in1 or in2 must be at least as large as the other in every dimension.
	///	same:	The output is the same size as in1, centered with respect to the �full� output.
	/// @param measure If true, run and time the convolution of in1 and in2 with both methods and return the fastest. If false (default), predict the fastest method using precomputed values.
	/// @return A pair containing an enum indicating which convolution method is fastest, either �direct� or �fft�, and a map containing the times (in seconds) needed for each method. This map is only non-empty if measure=true.
	template<class T>
	std::pair<convolution_method, std::map<convolution_method, double>>
		choose_conv_method(const std::vector<T>& in1, const std::vector<T>& in2,
			convolution_mode mode = convolution_mode::full, bool measure = false);

	/// @brief Convolve two N-dimensional arrays.
	///
	/// Convolve in1and in2, with the output size determined by the mode argument.
	/// @tparam T Type of the vector elements
	/// @param in1 First input
	/// @param in2 Second input
	/// full:	The output is the full discrete convolution of the inputs. (Default)
	///	valid: 	The output consists only of those elements that do not rely on the zero-padding. In �valid� mode, either in1 or in2 must be at least as large as the other in every dimension.
	///	same:	The output is the same size as in1, centered with respect to the �full� output.
	/// @param method An enum indicating which method to use to calculate the convolution.
	/// direct:	The correlation is determined directly from sums, the definition of correlation.
	/// fft:	The Fast Fourier Transform is used to perform the correlation more quickly.
	/// automatic: Automatically chooses direct or fft method based on an estimate of which is faster (default).
	/// @return A vector containing a subset of the discrete convolution of in1 with in2.
	template<class T>
	std::vector<T> convolve(const std::vector<T>& in1, const std::vector<T>& in2,
		convolution_mode mode = convolution_mode::full, convolution_method method = convolution_method::automatic);
	
	using correlation_mode = convolution_mode;
	using correlation_method = convolution_method;
	
	/// @brief Cross-correlate two vectors.
	/// @tparam T Type of the vector elements
	/// @param in1 First input
	/// @param in2 Second input
	/// @param mode An enum indicating the size of the output:
	/// full:	The output is the full discrete linear cross-correlation of the inputs. (Default)
	///	valid: 	The output consists only of those elements that do not rely on the zero-padding. In �valid� mode, either in1 or in2 must be at least as large as the other in every dimension.
	///	same:	The output is the same size as in1, centered with respect to the �full� output.
	/// @param method An enum indicating which method to use to calculate the correlation.
	/// direct:	The correlation is determined directly from sums, the definition of correlation.
	/// fft:	The Fast Fourier Transform is used to perform the correlation more quickly.
	/// automatic: Automatically chooses direct or fft method based on an estimate of which is faster (default).
	/// @return A vector containing a subset of the discrete linear cross-correlation of in1 with in2.
	template<class T>
	std::vector<T> correlate(const std::vector<T>& in1, const std::vector<T>& in2,
		correlation_mode mode = correlation_mode::full, correlation_method method = correlation_method::automatic);

	/// @brief Auto-correlate a vector with itself.
	/// @tparam T Type of the vector elements
	/// @param in Input vector
	/// @param mode An enum indicating the size of the output:
	/// full:	The output is the full discrete linear auto-correlation of the inputs. (Default)
	///	valid: 	The output consists only of those elements that do not rely on the zero-padding. In �valid� mode, either in1 or in2 must be at least as large as the other in every dimension.
	///	same:	The output is the same size as in1, centered with respect to the �full� output.
	/// @param method An enum indicating which method to use to calculate the correlation.
	/// direct:	The correlation is determined directly from sums, the definition of correlation.
	/// fft:	The Fast Fourier Transform is used to perform the correlation more quickly.
	/// automatic: Automatically chooses direct or fft method based on an estimate of which is faster (default).
	/// @return A vector containing a subset of the discrete linear auto-correlation of in.
	template<class T>
	std::vector<T> autocorrelate(const std::vector<T>& in, correlation_mode mode = correlation_mode::full, 
		correlation_method method = correlation_method::automatic)
	{
		return correlate(in, in, mode, method);
	}

	/// @brief Return the elements of a vector that satisfy some condition.
	/// @tparam T 
	/// @tparam Pred 
	/// @param condition A predicate which if true indicates the elements of vec to extract.
	/// @param vec Input vector
	/// @return Vector of values from vec where condition is true.
	template <class T, class Pred>
	std::vector<T> extract(Pred condition, const std::vector<T>& vec)
	{
		std::vector<T> results;

		auto it = std::find_if(vec.begin(), vec.end(), condition);
		while (it != vec.end())
		{
			results.push_back(*it);
			it = std::find_if(std::next(it), vec.end(), condition);
		}
		return results;
	}

	/// @brief Return evenly spaced numbers over a specified interval.
	///
	/// Returns N evenly spaced samples, calculated over the interval[start, stop].
	///
	/// The endpoint of the interval can optionally be excluded.
	/// @tparam T Type of the values in the vector.
	/// @param start The starting value of the sequence.
	/// @param stop The end value of the sequence, unless endpoint is set to False. In that case, the sequence consists of all but the last of num + 1 evenly spaced samples, so that stop is excluded. Note that the step size changes when endpoint is False.
	/// @param num Number of samples to generate. Default is 50.
	/// @param endpoint If True, stop is the last sample. Otherwise, it is not included. Default is True.
	/// @return Vector of evenly spaced numbers over the specified interval
	template <typename T>
	std::vector<T> linspace(T start, T stop, size_t num = 50, bool endpoint = true)
	{
		T h = (stop - start) / static_cast<T>(num - 1);
		std::vector<T> xs(num);

		typename std::vector<T>::iterator x;
		T val;
		for (x = xs.begin(), val = start; x != xs.end(); ++x, val += h)
		{
			*x = val;
		}
		if (!endpoint)
		{
			xs.pop_back();
		}
		return xs;
	}

	/// @brief Return modulus (which is not the same as the remainder for signed values)
	template <typename T>
	T mod(T x1, T x2)
	{
		if (x2 < 1) { x2 = 1; }

		if (x1 >= 0)
		{
			return static_cast<int>(x1) % static_cast<int>(x2);
		}

		return x2 - static_cast<int>(-x1) % static_cast<int>(x1);
	}
	
	/// @brief Return the next-largest power of two k so that n <= 2^k
	/// @param n Number of which find the next-largest power of two.
	/// @return The next-largest power of two so that n <= 2^k 
	unsigned nextpow2(unsigned n);
}
