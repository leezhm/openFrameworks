//
// SessionFactory.h
//
// $Id: //poco/Main/Data/include/Poco/Data/SessionFactory.h#7 $
//
// Library: Data
// Package: DataCore
// Module:  SessionFactory
//
// Definition of the SessionFactory class.
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


#ifndef Data_SessionFactory_INCLUDED
#define Data_SessionFactory_INCLUDED


#include "Poco/Data/Data.h"
#include "Poco/Data/Connector.h"
#include "Poco/Data/Session.h"
#include "Poco/Mutex.h"
#include "Poco/SharedPtr.h"
#include <map>


namespace Poco {
namespace Data {


class Data_API SessionFactory
	/// A SessionFactory is a singleton class that stores Connectors and allows to 
	/// create Sessions of the required type:
	///    
	///     Session ses(SessionFactory::instance().create(connector, connectionString));
	///    
	/// where the first param presents the type of session one wants to create (e.g. for SQLite one would choose "SQLite")
	/// and the second param is the connection string that the connector requires to connect to the database.
	///
	/// A concrete example to open an SQLite database stored in the file "dummy.db" would be
	///    
	///     Session ses(SessionFactory::instance().create(SQLite::Connector::KEY, "dummy.db"));
	///
	/// An even simpler way to create a session is to use the two argument constructor of Session, which
	/// automatically invokes the SessionFactory:
	///
	///      Session ses("SQLite", "dummy.db");
{
public:
	static SessionFactory& instance();
		/// returns the static instance of the singleton.

	void add(Connector* pIn);
		/// Registers a Connector under its key at the factory. If a registration for that
		/// key is already active, the first registration will be kept, only its reference count will be increased.
		/// Always takes ownership of parameter pIn.

	void remove(const std::string& key);
		/// Lowers the reference count for the Connector registered under that key. If the count reaches zero,
		/// the object is removed.

	Session create(const std::string& key,
		const std::string& connectionString,
		std::size_t timeout = Session::LOGIN_TIMEOUT_DEFAULT);
		/// Creates a Session for the given key with the connectionString. Throws an Poco:Data::UnknownDataBaseException
		/// if no Connector is registered for that key.

	Session create(const std::string& uri,
		std::size_t timeout = Session::LOGIN_TIMEOUT_DEFAULT);
		/// Creates a Session for the given URI (must be in key:///connectionString format). 
		///	Throws an Poco:Data::UnknownDataBaseException if no Connector is registered for the key.

private:
	SessionFactory();
	~SessionFactory();
	SessionFactory(const SessionFactory&);
	SessionFactory& operator = (const SessionFactory&);

	struct SessionInfo
	{
		int cnt;
		Poco::SharedPtr<Connector> ptrSI;
		SessionInfo(Connector* pSI);
	};

	typedef std::map<std::string, SessionInfo> Connectors;
	Connectors      _connectors;
	Poco::FastMutex _mutex;
};


} } // namespace Poco::Data


#endif // Data_SessionFactory_INCLUDED
