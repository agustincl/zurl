/*
 * Copyright (C) 2014 Fanout, Inc.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#include "httpheaders.h"

class QHostAddress;
class QUrl;
class QJDnsShared;

class WebSocket : public QObject
{
	Q_OBJECT

public:
	enum State
	{
		Idle,
		Connecting,
		Connected,
		Closing
	};

	enum ErrorCondition
	{
		ErrorNone,
		ErrorGeneric,
		ErrorPolicy,
		ErrorConnect,
		ErrorTls,
		ErrorRejected,
		ErrorFrameTooLarge,
		ErrorTimeout
	};

	class Frame
	{
	public:
		enum Type
		{
			Continuation,
			Text,
			Binary,
			Ping,
			Pong
		};

		Type type;
		QByteArray data;
		bool more;

		Frame(Type _type, const QByteArray &_data, bool _more) :
			type(_type),
			data(_data),
			more(_more)
		{
		}
	};

	WebSocket(QJDnsShared *dns, QObject *parent = 0);
	~WebSocket();

	void setConnectHost(const QString &host);
	void setIgnoreTlsErrors(bool on);
	void setMaxFrameSize(int size);

	void start(const QUrl &uri, const HttpHeaders &headers = HttpHeaders());

	State state() const;
	int responseCode() const;
	QByteArray responseReason() const;
	HttpHeaders responseHeaders() const;
	int framesAvailable() const;
	int nextFrameSize() const;
	int peerCloseCode() const;
	ErrorCondition errorCondition() const;

	// for rejections
	QByteArray readResponseBody();

	void writeFrame(const Frame &frame);
	Frame readFrame();
	void close(int code = -1);

signals:
	void nextAddress(const QHostAddress &addr);
	void connected();
	void readyRead();
	void framesWritten(int count);
	void peerClosing(); // emitted only if peer closes before we do
	void closed(); // emitted after peer acks our close, or immediately if we were acking
	void error();

private:
	class Private;
	friend class Private;
	Private *d;
};

#endif
