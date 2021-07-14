#include "authhandler.h"

#include <QDebug>
#include <QVariantMap>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QStringList>

AuthHandler::AuthHandler(QObject *parent)
    : QObject(parent)
    , m_apiKey( QString() )
{
    m_networkAccessManager = new QNetworkAccessManager( this );
    connect( this, &AuthHandler::userSignedIn, this, &AuthHandler::performAuthenticatedDatabaseCall );
}

AuthHandler::~AuthHandler()
{
    m_networkAccessManager->deleteLater();
}

void AuthHandler::setAPIKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void AuthHandler::signUserUp(const QString &emailAddress, const QString &password)
{
    QString signUpEndpoint = "https://qtfirebaseauthenticationeample-default-rtdb.firebaseio.com/Pets.json";

    QVariantMap variantPayload;
        variantPayload["Type"] = "dog";
        variantPayload["Name"] = "Mercry";
        variantPayload["Food"] = "chicken";


    QJsonDocument jsonPayload = QJsonDocument::fromVariant( variantPayload );
    performPOST( signUpEndpoint, jsonPayload );
}

void AuthHandler::signUserIn(const QString &emailAddress, const QString &password)
{
    QString signInEndpoint = "https://qtfirebaseauthenticationeample-default-rtdb.firebaseio.com/Pets.json";

    QVariantMap variantPayload;
        variantPayload["Type"] = "dog";
        variantPayload["Name"] = "Mercry";
        variantPayload["Food"] = "chicken";

    QJsonDocument jsonPayload = QJsonDocument::fromVariant( variantPayload );

    performPOST( signInEndpoint, jsonPayload );
}

void AuthHandler::networkReplyReadyRead()
{
    QByteArray response = m_networkReply->readAll();
    //qDebug() << response;
    m_networkReply->deleteLater();

    parseResponse( response );
}

void AuthHandler::performAuthenticatedDatabaseCall()
{
    QString endPoint = "http://172.16.60.215/users/createUser" + m_idToken;
    m_networkReply = m_networkAccessManager->get( QNetworkRequest(QUrl(endPoint)));
    connect( m_networkReply, &QNetworkReply::readyRead, this, &AuthHandler::networkReplyReadyRead );
}

void AuthHandler::performPOST(const QString &url, const QJsonDocument &payload)
{
    QNetworkRequest newRequest( (QUrl( url )) );
    newRequest.setHeader( QNetworkRequest::ContentTypeHeader, QString( "application/json"));
    m_networkReply = m_networkAccessManager->post( newRequest, payload.toJson());
    connect( m_networkReply, &QNetworkReply::readyRead, this, &AuthHandler::networkReplyReadyRead );
}

void AuthHandler::parseResponse(const QByteArray &response)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson( response );
    if ( jsonDocument.object().contains("error") )
    {
        qDebug() << "Error occured!" << response;
    }
    else if ( jsonDocument.object().contains("kind"))
    {
        QString idToken = jsonDocument.object().value("idToken").toString();
        //qDebug() << "Obtained user ID Token: " << idToken;
        qDebug() << "User signed in successfully!";
        m_idToken = idToken;
        emit userSignedIn();
    }
    else
        qDebug() << "The response was: " << response;
}
