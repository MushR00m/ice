// **********************************************************************
//
// Copyright (c) 2001
// MutableRealms, Inc.
// Huntsville, AL, USA
//
// All Rights Reserved
//
// **********************************************************************

#include <Ice/TraceUtil.h>
#include <Ice/Instance.h>
#include <Ice/Object.h>
#include <Ice/Proxy.h>
#include <Ice/TraceLevels.h>
#include <Ice/Logger.h>
#include <Ice/BasicStream.h>
#include <Ice/Protocol.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

static void
printHeader(ostream& s, BasicStream& stream)
{
    Byte protVer;
    stream.read(protVer);
    s << "\nprotocol version = " << static_cast<int>(protVer);
    Byte encVer;
    stream.read(encVer);
    s << "\nencoding version = " << static_cast<int>(encVer);
    Byte type;
    stream.read(type);
    s << "\nmessage type = "  << static_cast<int>(type) << ' ';
    switch(type)
    {
	case requestMsg:
	{
	    s << "(request)";
	    break;
	}
	case requestBatchMsg:
	{
	    s << "(batch request)";
	    break;
	}
	case replyMsg:
	{
	    s << "(reply)";
	    break;
	}
	case closeConnectionMsg:
	{
	    s << "(close connection)";
	    break;
	}
	default:
	{
	    s << "(unknown)";
	    break;
	}
    }
    Int size;
    stream.read(size);
    s << "\nmessage size = " << size;
}

static void
printRequestHeader(ostream& s, BasicStream& stream)
{
    string identity;
    string facet;
    bool gotProxy;
    stream.read(gotProxy);
    s << "\naddressing = " << static_cast<int>(gotProxy);
    if (gotProxy)
    {
	s << " (proxy)";
	ObjectPrx proxy;
	stream.read(proxy);
	identity = proxy->ice_getIdentity();
	facet = proxy->ice_getFacet();
    }
    else
    {
	s << " (identity)";
	stream.read(identity);
	stream.read(facet);
    }
    s << "\nidentity = " << identity;
    s << "\nfacet = " << facet;
    string operation;
    stream.read(operation);
    s << "\noperation = " << operation;
    bool nonmutating;
    stream.read(nonmutating);
    s << "\nnonmutating = " << (nonmutating ? "true" : "false");
    Int sz;
    stream.read(sz);
    s << "\ncontext = ";
    while (sz--)
    {
	pair<string, string> pair;
	stream.read(pair.first);
	stream.read(pair.second);
	s << pair.first << '/' << pair.second;
	if (sz)
	{
	    s << ", ";
	}
    }
}

void
IceInternal::traceHeader(const char* heading, const BasicStream& str, const ::Ice::LoggerPtr& logger,
			 const TraceLevelsPtr& tl)
{
    if (tl->protocol >= 1)
    {
	BasicStream& stream = const_cast<BasicStream&>(str);
	BasicStream::Container::iterator p = stream.i;
	stream.i = stream.b.begin();
	ostringstream s;
	s << heading;
	printHeader(s, stream);
	logger->trace(tl->protocolCat, s.str());
	stream.i = p;
    }
}

void
IceInternal::traceRequest(const char* heading, const BasicStream& str, const ::Ice::LoggerPtr& logger,
			  const TraceLevelsPtr& tl)
{
    if (tl->protocol >= 1)
    {
	BasicStream& stream = const_cast<BasicStream&>(str);
	BasicStream::Container::iterator p = stream.i;
	stream.i = stream.b.begin();
	ostringstream s;
	s << heading;
	printHeader(s, stream);
	Int requestId;
	stream.read(requestId);
	s << "\nrequest id = " << requestId;
	if (requestId == 0)
	{
	    s << " (oneway)";
	}
	printRequestHeader(s, stream);
	logger->trace(tl->protocolCat, s.str());
	stream.i = p;
    }
}

void
IceInternal::traceBatchRequest(const char* heading, const BasicStream& str, const ::Ice::LoggerPtr& logger,
			       const TraceLevelsPtr& tl)
{
    if (tl->protocol >= 1)
    {
	BasicStream& stream = const_cast<BasicStream&>(str);
	BasicStream::Container::iterator p = stream.i;
	stream.i = stream.b.begin();
	ostringstream s;
	s << heading;
	printHeader(s, stream);
	int cnt = 0;
	while (stream.i != stream.b.end())
	{
	    s << "\nrequest #" << cnt++ << ':';
	    printRequestHeader(s, stream);
	    stream.skipEncaps();
	}
	logger->trace(tl->protocolCat, s.str());
	stream.i = p;
    }
}

void
IceInternal::traceReply(const char* heading, const BasicStream& str, const ::Ice::LoggerPtr& logger,
			const TraceLevelsPtr& tl)
{
    if (tl->protocol >= 1)
    {
	BasicStream& stream = const_cast<BasicStream&>(str);
	BasicStream::Container::iterator p = stream.i;
	stream.i = stream.b.begin();
	ostringstream s;
	s << heading;
	printHeader(s, stream);
	Int requestId;
	stream.read(requestId);
	s << "\nrequest id = " << requestId;
	Byte status;
	stream.read(status);
	s << "\nreply status = " << static_cast<int>(status) << ' ';
	switch(static_cast<DispatchStatus>(status))
	{
	    case DispatchOK:
	    {
		s << "(ok)";
		break;
	    }
	    case DispatchUserException:
	    {
		s << "(user exception)";
		break;
	    }
	    case DispatchLocationForward:
	    {
		s << "(location forward)";
		break;
	    }
	    case DispatchProxyRequested:
	    {
		s << "(proxy requested)";
		break;
	    }
	    case DispatchObjectNotExist:
	    {
		s << "(object not exist)";
		break;
	    }
	    case DispatchOperationNotExist:
	    {
		s << "(operation not exist)";
		break;
	    }
	    case DispatchUnknownLocalException:
	    {
		s << "(unknown local exception)";
		break;
	    }
	    case DispatchUnknownUserException:
	    {
		s << "(unknown user exception)";
		break;
	    }
	    case DispatchUnknownException:
	    {
		s << "(unknown exception)";
		break;
	    }
	    default:
	    {
		s << "(unknown)";
		break;
	    }
	}
	logger->trace(tl->protocolCat, s.str());
	stream.i = p;
    }
}
