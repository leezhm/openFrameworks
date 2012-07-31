//
// Statement.h
//
// $Id: //poco/Main/Data/include/Poco/Data/Statement.h#18 $
//
// Library: Data
// Package: DataCore
// Module:  Statement
//
// Definition of the Statement class.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Data_Statement_INCLUDED
#define Data_Statement_INCLUDED


#include "Poco/Data/Data.h"
#include "Poco/Data/StatementImpl.h"
#include "Poco/Data/Range.h"
#include "Poco/Data/Bulk.h"
#include "Poco/Data/Row.h"
#include "Poco/Data/SimpleRowFormatter.h"
#include "Poco/SharedPtr.h"
#include "Poco/Mutex.h"
#include "Poco/ActiveMethod.h"
#include "Poco/ActiveResult.h"
#include "Poco/Format.h"


namespace Poco {
namespace Data {


class AbstractBinding;
class AbstractExtraction;
class Session;
class Limit;


class Data_API Statement
	/// A Statement is used to execute SQL statements. 
	/// It does not contain code of its own.
	/// Its main purpose is to forward calls to the concrete StatementImpl stored inside.
	/// Statement execution can be synchronous or asynchronous.
	/// Synchronous ececution is achieved through execute() call, while asynchronous is
	/// achieved through executeAsync() method call.
	/// An asynchronously executing statement should not be copied during the execution. 
	///
	/// Note:
	///
	/// Once set as asynchronous through 'async' manipulator, statement remains
	/// asynchronous for all subsequent execution calls, both execute() and executeAsync().
	/// However, calling executAsync() on a synchronous statement shall execute 
	/// asynchronously but without altering the underlying statement's synchronous nature.
	///
	/// Once asyncronous, a statement can be reverted back to synchronous state in two ways:
	/// 
	///	1) By calling setAsync(false)
	///	2) By means of 'sync' or 'reset' manipulators
	///
	/// See individual functions documentation for more details.
	///
	/// Statement owns the RowFormatter, which can be provided externaly through setFormatter()
	/// member function.
	/// If no formatter is externally supplied to the statement, the SimpleRowFormatter is lazy
	/// created and used.
{
public:
	typedef void (*Manipulator)(Statement&);

	typedef ActiveResult<std::size_t>                      Result;
	typedef SharedPtr<Result>                              ResultPtr;
	typedef ActiveMethod<std::size_t, bool, StatementImpl> AsyncExecMethod;
	typedef SharedPtr<AsyncExecMethod>                     AsyncExecMethodPtr;

	static const int WAIT_FOREVER = -1;

	enum Storage
	{
		STORAGE_DEQUE   = StatementImpl::STORAGE_DEQUE_IMPL,
		STORAGE_VECTOR  = StatementImpl::STORAGE_VECTOR_IMPL,
		STORAGE_LIST    = StatementImpl::STORAGE_LIST_IMPL,
		STORAGE_UNKNOWN = StatementImpl::STORAGE_UNKNOWN_IMPL
	};

	Statement(StatementImpl* pImpl);
		/// Creates the Statement.

	explicit Statement(Session& session);
		/// Creates the Statement for the given Session.
		///
		/// The following:
		///
		///     Statement stmt(sess);
		///     stmt << "SELECT * FROM Table", ...
		///
		/// is equivalent to:
		/// 
		///     Statement stmt(sess << "SELECT * FROM Table", ...);
		///
		/// but in some cases better readable.

	~Statement();
		/// Destroys the Statement.

	Statement(const Statement& stmt);
		/// Copy constructor. 
		/// If the statement has been executed asynchronously and has not been
		/// synchronized prior to copy operation (i.e. is copied while executing), 
		/// this constructor shall synchronize it.

	Statement& operator = (const Statement& stmt);
		/// Assignment operator.

	void swap(Statement& other);
		/// Swaps the statement with another one.

	template <typename T> 
	Statement& operator << (const T& t)
		/// Concatenates data with the SQL statement string.
	{
		_pImpl->add(t);
		return *this;
	}

	Statement& operator , (Manipulator manip);
		/// Handles manipulators, such as now, async, etc.

	Statement& operator , (AbstractBinding* bind);
		/// Registers the Binding with the Statement by calling addBind().
		/// Statement takes the ownership of the bind in an AutoPtr.
		/// To prevent bind destruction upon statement destruction, pass an 
		/// AutoPtr<AbstractBinding>::duplicate() to this function.
		/// This function is primarily intended to be a destination for 
		/// use() and bind() utility functions return value, so it is 
		/// recommended to call addBind() directly instead.

	Statement& addBind(AbstractBinding* pBind, bool duplicate = true);
		/// Registers a single binding with the statement.
		/// To allow the binding to outlive the statement destruction,
		/// duplicate must be true.

	void removeBind(const std::string& name);
		/// Removes the all the bindings with specified name from the statement.

	Statement& operator , (AbstractBindingVec& bindVec);
		/// Registers the Binding vector with the Statement.

	Statement& operator , (AbstractExtraction* extract);
		/// Registers objects used for extracting data with the Statement by 
		/// calling addExtract().
		/// Statement takes the ownership of the extract in an AutoPtr.
		/// To prevent extract destruction upon statement destruction, pass an 
		/// AutoPtr<AbstractExtraction>::duplicate() to this function.
		/// This function is primarily intended to be a destination for 
		/// into() utility function return value, so it is recommended to call 
		/// addExtract() directly instead.

	Statement& addExtract(AbstractExtraction* pExtract, bool duplicate = true);
		/// Registers a single extraction with the statement.
		/// To allow the extraction to outlive the statement destruction,
		/// duplicate must be true.

	Statement& operator , (AbstractExtractionVec& extVec);
		/// Registers the extraction vector with the Statement.
		/// The vector is registered at position 0 (i.e. for the first returned data set).

	Statement& operator , (AbstractExtractionVecVec& extVecVec);
		/// Registers the vector of extraction vectors with the Statement.

	template <typename C>
	Statement& addBinding(C& bindingCont, bool reset)
		/// Registers binding container with the Statement.
	{
		if (reset) _pImpl->resetBinding();
		typename C::iterator itAB = bindingCont.begin();
		typename C::iterator itABEnd = bindingCont.end();
		for (; itAB != itABEnd; ++itAB)	addBind(itAB->duplicate());
		return *this;
	}

	template <typename C>
	Statement& addExtraction(C& val, bool reset)
		/// Registers extraction container with the Statement.
	{
		if (reset) _pImpl->resetExtraction();
		typename C::iterator itAE = val.begin();
		typename C::iterator itAEEnd = val.end();
		for (; itAE != itAEEnd; ++itAE)	addExtract(itAE->duplicate());
		return *this;
	}

	template <typename C>
	Statement& addExtractions(C& val)
		/// Registers container of extraction containers with the Statement.
	{
		_pImpl->resetExtraction();
		typename C::iterator itAEV = val.begin();
		typename C::iterator itAEVEnd = val.end();
		for (; itAEV != itAEVEnd; ++itAEV) addExtraction(*itAEV, false);
		return *this;
	}

	
	Statement& operator , (const Bulk& bulk);
		/// Sets the bulk execution mode (both binding and extraction) for this 
		/// statement.Statement must not have any extractors or binders set at the 
		/// time when this operator is applied. 
		/// Failure to adhere to the above constraint shall result in 
		/// InvalidAccessException.

	Statement& operator , (BulkFnType);
		/// Sets the bulk execution mode (both binding and extraction) for this 
		/// statement.Statement must not have any extractors or binders set at the 
		/// time when this operator is applied. 
		/// Additionally, this function requires limit to be set in order to
		/// determine the bulk size.
		/// Failure to adhere to the above constraints shall result in 
		/// InvalidAccessException.

	Statement& operator , (const Limit& extrLimit);
		/// Sets a limit on the maximum number of rows a select is allowed to return.
		///
		/// Set per default to zero to Limit::LIMIT_UNLIMITED, which disables the limit.

	Statement& operator , (RowFormatter* pRowFformatter);
		/// Sets the row formatter for the statement.

	Statement& operator , (const Range& extrRange);
		/// Sets a an extraction range for the maximum number of rows a select is allowed to return.
		///
		/// Set per default to Limit::LIMIT_UNLIMITED which disables the range.

	Statement& operator , (char value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (Poco::UInt8 value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (Poco::Int8 value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (Poco::UInt16 value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (Poco::Int16 value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (Poco::UInt32 value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (Poco::Int32 value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

#ifndef POCO_LONG_IS_64_BIT
	Statement& operator , (long value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (unsigned long value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.
#endif
	Statement& operator , (Poco::UInt64 value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (Poco::Int64 value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (double value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (float value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (bool value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (const std::string& value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	Statement& operator , (const char* value);
		/// Adds the value to the list of values to be supplied to the SQL string formatting function.

	const std::string& toString() const;
		/// Creates a string from the accumulated SQL statement.

	std::size_t execute(bool reset = true);
		/// Executes the statement synchronously or asynchronously. 
		/// Stops when either a limit is hit or the whole statement was executed.
		/// Returns the number of rows extracted from the database (for statements
		/// returning data) or number of rows affected (for all other statements).
		/// If reset is true (default), associated storage is reset and reused.
		/// Otherwise, the results from this execution step are appended.
		/// Reset argument has no meaning for unlimited statements that return all rows.
		/// If isAsync() returns  true, the statement is executed asynchronously 
		/// and the return value from this function is zero.
		/// The result of execution (i.e. number of returned or affected rows) can be 
		/// obtained by calling wait() on the statement at a later point in time.

	const Result& executeAsync(bool reset = true);
		/// Executes the statement asynchronously. 
		/// Stops when either a limit is hit or the whole statement was executed.
		/// Returns immediately. Calling wait() (on either the result returned from this 
		/// call or the statement itself) returns the number of rows extracted or number 
		/// of rows affected by the statement execution.
		/// When executed on a synchronous statement, this method does not alter the
		/// statement's synchronous nature.

	void setAsync(bool async = true);
		/// Sets the asynchronous flag. If this flag is true, executeAsync() is called 
		/// from the now() manipulator. This setting does not affect the statement's
		/// capability to be executed synchronously by directly calling execute().

	bool isAsync() const;
		/// Returns true if statement was marked for asynchronous execution.

	std::size_t wait(long milliseconds = WAIT_FOREVER);
		/// Waits for the execution completion for asynchronous statements or
		/// returns immediately for synchronous ones. The return value for 
		/// asynchronous statement is the execution result (i.e. number of 
		/// rows retrieved). For synchronous statements, the return value is zero.

	bool initialized();
		/// Returns true if the statement was initialized (i.e. not executed yet).

	bool paused();
		/// Returns true if the statement was paused (a range limit stopped it
		/// and there is more work to do).

	bool done();
		/// Returns true if the statement was completely executed or false if a range limit stopped it
		/// and there is more work to do. When no limit is set, it will always return true after calling execute().

	Statement& reset(Session& session);
		/// Resets the Statement so that it can be filled with a new SQL command.

	bool canModifyStorage();
		/// Returns true if statement is in a state that allows the internal storage to be modified.

	Storage storage() const;
		/// Returns the internal storage type for the stamement.

	void setStorage(const std::string& storage);
		/// Sets the internal storage type for the stamement.

	const std::string& getStorage() const;
		/// Returns the internal storage type for the stamement.

	std::size_t columnsExtracted(int dataSet = StatementImpl::USE_CURRENT_DATA_SET) const;
		/// Returns the number of columns returned for current data set.
		/// Default value indicates current data set (if any).

	std::size_t rowsExtracted(int dataSet = StatementImpl::USE_CURRENT_DATA_SET) const;
		/// Returns the number of rows returned for current data set during last statement
		/// execution. Default value indicates current data set (if any).

	std::size_t subTotalRowCount(int dataSet = StatementImpl::USE_CURRENT_DATA_SET) const;
		/// Returns the number of rows extracted so far for the data set.
		/// Default value indicates current data set (if any).

	std::size_t extractionCount() const;
		/// Returns the number of extraction storage buffers associated
		/// with the current data set.

	std::size_t dataSetCount() const;
		/// Returns the number of data sets associated with the statement.

	std::size_t nextDataSet();
		/// Returns the index of the next data set.

	std::size_t previousDataSet();
		/// Returns the index of the previous data set.

	bool hasMoreDataSets() const;
		/// Returns false if the current data set index points to the last
		/// data set. Otherwise, it returns true.

	void setRowFormatter(RowFormatter* pRowFormatter);
		/// Sets the row formatter for this statement.
		/// Statement takes the ownership of the formatter.

protected:
	typedef Poco::AutoPtr<StatementImpl> StatementImplPtr;

	const AbstractExtractionVec& extractions() const;
		/// Returns the extractions vector.

	const MetaColumn& metaColumn(std::size_t pos) const;
		/// Returns the type for the column at specified position.

	const MetaColumn& metaColumn(const std::string& name) const;
		/// Returns the type for the column with specified name.

	 bool isNull(std::size_t col, std::size_t row) const;
		/// Returns true if the current row value at column pos is null.

	 bool isBulkExtraction() const;
		/// Returns true if this statement extracts data in bulk.

	StatementImplPtr impl() const;
		/// Returns pointer to statement implementation.

	const RowFormatterPtr& getRowFormatter();
		/// Returns the row formatter for this statement.

	Session session();
		/// Returns the underlying session.

private:
	const Result& doAsyncExec(bool reset = true);
		/// Asynchronously executes the statement.

	template <typename T>
	Statement& commaPODImpl(const T& val)
	{
		_arguments.push_back(val);
		return *this;
	}

	StatementImplPtr _pImpl;

	// asynchronous execution related members
	bool                _async;
	mutable ResultPtr   _pResult;
	Mutex               _mutex;
	AsyncExecMethodPtr  _pAsyncExec;
	std::vector<Any>    _arguments;
	RowFormatterPtr     _pRowFormatter;
	mutable std::string _stmtString;
};

//
// inlines

inline std::size_t Statement::subTotalRowCount(int dataSet) const
{
	return _pImpl->subTotalRowCount(dataSet);
}


namespace Keywords {


//
// Manipulators
//

inline void Data_API now(Statement& statement)
	/// Enforces immediate execution of the statement.
	/// If _isAsync flag has been set, execution is invoked asynchronously.
{
	statement.execute();
}


inline void Data_API sync(Statement& statement)
	/// Sets the _isAsync flag to false, signalling synchronous execution.
	/// Synchronous execution is default, so specifying this manipulator
	/// only makes sense if async() was called for the statement before.
{
	statement.setAsync(false);
}


inline void Data_API async(Statement& statement)
	/// Sets the _async flag to true, signalling asynchronous execution.
{
	statement.setAsync(true);
}


inline void Data_API deque(Statement& statement)
	/// Sets the internal storage to std::deque.
	/// std::deque is default storage, so specifying this manipulator
	/// only makes sense if list() or deque() were called for the statement before.
{
	if (!statement.canModifyStorage())
		throw InvalidAccessException("Storage not modifiable.");

	statement.setStorage("deque");
}


inline void Data_API vector(Statement& statement)
	/// Sets the internal storage to std::vector.
{
	if (!statement.canModifyStorage())
		throw InvalidAccessException("Storage not modifiable.");

	statement.setStorage("vector");
}


inline void Data_API list(Statement& statement)
	/// Sets the internal storage to std::list.
{
	if (!statement.canModifyStorage())
		throw InvalidAccessException("Storage not modifiable.");

	statement.setStorage("list");
}


inline void Data_API reset(Statement& statement)
	/// Sets all internal settings to their respective default values.
{
	if (!statement.canModifyStorage())
		throw InvalidAccessException("Storage not modifiable.");

	statement.setStorage("deque");
	statement.setAsync(false);
}


} // namespace Keywords


//
// inlines
//

inline Statement& Statement::operator , (RowFormatter* pRowFformatter)
{
	_pRowFormatter = pRowFformatter;
	return *this;
}


inline Statement& Statement::operator , (char value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (Poco::UInt8 value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (Poco::Int8 value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (Poco::UInt16 value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (Poco::Int16 value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (Poco::UInt32 value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (Poco::Int32 value)
{
	return commaPODImpl(value);
}


#ifndef POCO_LONG_IS_64_BIT
inline Statement& Statement::operator , (long value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (unsigned long value)
{
	return commaPODImpl(value);
}
#endif


inline Statement& Statement::operator , (Poco::UInt64 value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (Poco::Int64 value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (double value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (float value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (bool value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (const std::string& value)
{
	return commaPODImpl(value);
}


inline Statement& Statement::operator , (const char* value)
{
	return commaPODImpl(std::string(value));
}


inline void Statement::removeBind(const std::string& name)
{
	_pImpl->removeBind(name);
}


inline Statement& Statement::operator , (AbstractBinding* pBind)
{
	return addBind(pBind, false);
}


inline Statement& Statement::operator , (AbstractBindingVec& bindVec)
{
	return addBinding(bindVec, false);
}


inline Statement& Statement::operator , (AbstractExtraction* pExtract)
{
	return addExtract(pExtract, false);
}


inline Statement& Statement::operator , (AbstractExtractionVec& extVec)
{
	return addExtraction(extVec, false);
}


inline Statement& Statement::operator , (AbstractExtractionVecVec& extVecVec)
{
	return addExtractions(extVecVec);
}


inline Statement::StatementImplPtr Statement::impl() const
{
	return _pImpl;
}


inline const std::string& Statement::toString() const
{
	return _stmtString = _pImpl->toString();
}

inline const AbstractExtractionVec& Statement::extractions() const
{
	return _pImpl->extractions();
}


inline const MetaColumn& Statement::metaColumn(std::size_t pos) const
{
	return _pImpl->metaColumn(pos);
}


inline const MetaColumn& Statement::metaColumn(const std::string& name) const
{
	return _pImpl->metaColumn(name);
}


inline void Statement::setStorage(const std::string& storage)
{
	_pImpl->setStorage(storage);
}


inline std::size_t Statement::extractionCount() const
{
	return _pImpl->extractionCount();
}


inline std::size_t Statement::columnsExtracted(int dataSet) const
{
	return _pImpl->columnsExtracted(dataSet);
}


inline std::size_t Statement::rowsExtracted(int dataSet) const
{
	return _pImpl->rowsExtracted(dataSet);
}


inline std::size_t Statement::dataSetCount() const
{
	return _pImpl->dataSetCount();
}


inline std::size_t Statement::nextDataSet()
{
	return _pImpl->activateNextDataSet();
}


inline std::size_t Statement::previousDataSet()
{
	return _pImpl->activatePreviousDataSet();
}


inline bool Statement::hasMoreDataSets() const
{
	return _pImpl->hasMoreDataSets();
}


inline Statement::Storage Statement::storage() const
{
	return static_cast<Storage>(_pImpl->getStorage());
}


inline bool Statement::canModifyStorage()
{
	return (0 == extractionCount()) && (initialized() || done());
}


inline bool Statement::initialized()
{
	return _pImpl->getState() == StatementImpl::ST_INITIALIZED;
}


inline bool Statement::paused()
{
	return _pImpl->getState() == StatementImpl::ST_PAUSED;
}


inline bool Statement::done()
{
	return _pImpl->getState() == StatementImpl::ST_DONE;
}


inline bool Statement::isNull(std::size_t col, std::size_t row) const
{
	return _pImpl->isNull(col, row);
}


inline bool Statement::isBulkExtraction() const
{
	return _pImpl->isBulkExtraction();
}


inline bool Statement::isAsync() const
{
	return _async;
}


inline void Statement::setRowFormatter(RowFormatter* pRowFormatter)
{
	_pRowFormatter = pRowFormatter;
}


inline const RowFormatterPtr& Statement::getRowFormatter()
{
	if (!_pRowFormatter) _pRowFormatter = new SimpleRowFormatter;
	return _pRowFormatter;
}


inline void swap(Statement& s1, Statement& s2)
{
	s1.swap(s2);
}


} } // namespace Poco::Data


namespace std
{
	template<>
	inline void swap<Poco::Data::Statement>(Poco::Data::Statement& s1, 
		Poco::Data::Statement& s2)
		/// Full template specalization of std:::swap for Statement
	{
		s1.swap(s2);
	}
}


#endif // Data_Statement_INCLUDED
