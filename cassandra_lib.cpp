/*
 * License:
 *      Copyright (c) 2012-2013 Made to Order Software Corp.
 * 
 *      http://snapwebsites.org/
 *      contact@m2osw.com
 * 
 *      Permission is hereby granted, free of charge, to any person obtaining a
 *      copy of this software and associated documentation files (the
 *      "Software"), to deal in the Software without restriction, including
 *      without limitation the rights to use, copy, modify, merge, publish,
 *      distribute, sublicense, and/or sell copies of the Software, and to
 *      permit persons to whom the Software is furnished to do so, subject to
 *      the following conditions:
 *
 *      The above copyright notice and this permission notice shall be included
 *      in all copies or substantial portions of the Software.
 *
 *      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *      CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *      TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *      SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <QtCassandra/QCassandra.h>
#include <QtCore/QDebug>
#include <qjson/serializer.h>
#include <qjson/parser.h>
#include <thrift-gencpp-cassandra/cassandra_types.h>
#include <openssl/sha.h>
#include "32_cassandra.h"
#include "utils.h"

/* Globals */
QtCassandra::QCassandra                        cassandra; 
QSharedPointer<QtCassandra::QCassandraContext> context; 

QSharedPointer<QtCassandra::QCassandraTable> master_table;
QSharedPointer<QtCassandra::QCassandraTable> user_table;
QSharedPointer<QtCassandra::QCassandraTable> video_table;
QSharedPointer<QtCassandra::QCassandraTable> employer_table;
QSharedPointer<QtCassandra::QCassandraTable> resume_table;
QSharedPointer<QtCassandra::QCassandraTable> salts_table;

char master_columns[][MAX_SIZE]={"id","UUIDType","email","AsciiType",
    "password","AsciiType",
    "is_employer","BooleanType", "stop","stop"};

char users_columns[][MAX_SIZE]={"id","UUIDType","firstname","AsciiType",
    "lastname","AsciiType","paid","integerType", 
    "pc","BytesType","stop","stop"};

char employers_columns[][MAX_SIZE]={"id","UUIDType","name","AsciiType",
    "employer_info","AsciiType","paid","IntegerType",
    "pc","BytesType","jobs","BytesType",
    "stop","stop"};

char resume_columns[][MAX_SIZE]={"id","UUIDType","resume_info","AsciiType",
    "stop","stop"};

char video_columns[][MAX_SIZE]={"id","UUIDType","video_info","AsciiType",
    "stop","stop"};

char salts_columns[][MAX_SIZE]={"id","UUIDType","salt","AsciiType",
    "stop","stop"};


const char *keyspace = KEYSPACE;

/* Debug function to dump all tables and column definitions*/
    void
libcass_dump_data ()
{
    const QtCassandra::QCassandraContexts& contexts = cassandra.contexts();
    for(QtCassandra::QCassandraContexts::const_iterator
            c = contexts.begin(); c != contexts.end(); ++c)
    {
        QString context_name = (*c)->contextName();

        if (context_name.compare(keyspace) != 0) 
            continue;

        qDebug() << "  + Context Name" << context_name;

        qDebug() << "    Strategy class" << (*c)->strategyClass();
        const QtCassandra::QCassandraContext::QCassandraContextOptions 
            options = (*c)->descriptionOptions();
        for(QtCassandra::QCassandraContext::QCassandraContextOptions::const_iterator
                o = options.begin(); o != options.end(); ++o)
        {
            qDebug() << "    + Option" << o.key() << "=" << o.value();
        }
        qDebug() << "    Replication Factor:" << (*c)->replicationFactor();
        qDebug() << "    Durable Writes:" << (*c)->durableWrites();

        const QtCassandra::QCassandraTables& tables = (*c)->tables();
        for(QtCassandra::QCassandraTables::const_iterator
                t = tables.begin(); t != tables.end(); ++t)
        {
            qDebug() << "    + Table" << (*t)->tableName() << "/" << 
                (*t)->identifier() << " (From Context" << (*t)->contextName() << ")";
            QString comment = (*t)->comment();
            if(!comment.isEmpty()) {
                qDebug() << "      Comment:" << comment;
            }
            qDebug() << "      Column Type" << (*t)->columnType();
            qDebug() << "      Default Validation Class" << (*t)->defaultValidationClass();
            QString key_alias = (*t)->keyAlias();
            if(key_alias.isEmpty()) {
                qDebug() << "      Key Validation Class" << (*t)->keyValidationClass();
            }
            else {
                qDebug() << "      Key Validation Class" << 
                    (*t)->keyValidationClass() << "and alias" << key_alias;
            }
            QString subcomparator = (*t)->subcomparatorType();
            if(subcomparator.isEmpty()) {
                qDebug() << "      Comparator Type" << (*t)->comparatorType();
            }
            else {
                qDebug() << "      Comparator Type" << 
                    (*t)->comparatorType() << "and subtype" << subcomparator;
            }
            qDebug() << "      Row Cache Provider" << (*t)->rowCacheProvider();
            qDebug() << "      Row Cache Size" << 
                (*t)->rowCacheSize() << "for" << 
                (*t)->rowCacheSavePeriodInSeconds() << "seconds";
            qDebug() << "      Key Cache Size" << 
                (*t)->keyCacheSize() << "for" << 
                (*t)->keyCacheSavePeriodInSeconds() << "seconds";
            qDebug() << "      Read repair chance" << (*t)->readRepairChance();
            qDebug() << "      Compaction Threshold: minimum" << 
                (*t)->minCompactionThreshold() << "maximum" << 
                (*t)->maxCompactionThreshold();
            qDebug() << "      Replicate on Write" << (*t)->replicateOnWrite();
            qDebug() << "      Merge Shards Chance" << (*t)->mergeShardsChance();
            qDebug() << "      Garbage Collection Grace Period" << 
                (*t)->gcGraceSeconds() << "seconds";
            qDebug() << "      Memory Tables Size (Mb)" << (*t)->memtableThroughputInMb()
                << "Flush After (min.)" << (*t)->memtableFlushAfterMins()
                << "Operations in Millions" << (*t)->memtableOperationsInMillions();

            const QtCassandra::QCassandraRows& r(((*t)->rows()));
            QString row_name;
            for(QtCassandra::QCassandraRows::const_iterator o(r.begin()); o != r.end(); ++o) {
                row_name = o.value()->rowName();
                qDebug() << "Row Names:" << row_name;
            }

            const QtCassandra::QCassandraColumnDefinitions& columns = (*t)->columnDefinitions();
            if(columns.begin() == columns.end()) {
                qDebug() << "      No column defintions";
            }
            for(QtCassandra::QCassandraColumnDefinitions::const_iterator
                    col = columns.begin(); col != columns.end(); ++col)
            {
                qDebug() << "      + Column" << (*col)->columnName();
                qDebug() << "        Validation Class" << (*col)->validationClass();
                QString type;
                switch((*col)->indexType()) {
                    case QtCassandra::QCassandraColumnDefinition::INDEX_TYPE_KEYS:
                        type = "KEYS";
                        break;

                    default: // unknown
                        type = "Unknown";
                        break;

                }
                qDebug() << "        Index Type" << (*col)->indexType()
                    << ("(" + type + ")");
                qDebug() << "        Index Name" << (*col)->indexName();
            }
        }

    }
}
    int 
libcass_table_create (const char *tablename, const char *col_type,
                      const char *validation_class, const char *comparator_type, 
                      QSharedPointer <QtCassandra::QCassandraTable>& table,
                      char ptr[][MAX_SIZE])
{
    QSharedPointer<QtCassandra::QCassandraColumnDefinition> column;
    int i ;

    if (tablename == NULL || col_type == NULL || 
            validation_class == NULL || comparator_type == NULL) {
        return(E_PARAM_PROBLEM);
    }

    table = context->findTable(tablename);

    /* Intialize the column definitions */
    i = 0 ;
    while(1) { 
        if (strncmp(ptr[i],STOP,STOPSIZE) == 0) 
            break;
        column = table->columnDefinition(ptr[i]); 
        column->setValidationClass(ptr[i+1]);

        i += 2;
    }
    return (E_OK);

}

    int 
libcass_init (const char *host, const char *strategy, 
              int repl_factor)
{
    if (host == NULL || strategy == NULL || keyspace == NULL) {
        goto done;
    }
    cassandra.connect(host);
    qDebug() << "Working on Cassandra Cluster Named" << cassandra.clusterName();
    qDebug() << "Working on Cassandra Protocol Version" << cassandra.protocolVersion();

    context = cassandra.context(keyspace);

    if (libcass_table_create("master","Standard","BytesType",
                "BytesType",master_table, master_columns) == -1) {
        goto done;
    }

    if (libcass_table_create("users","Standard","BytesType",
                "BytesType",user_table, users_columns) == -1) {
        goto done;
    }

    if (libcass_table_create("employers","Standard","BytesType",
                "BytesType", employer_table, employers_columns) == -1) {
        goto done;
    }   

    if (libcass_table_create("resumes","Standard","BytesType",
                "BytesType", resume_table, resume_columns) == -1) {
        goto done;
    }
    if (libcass_table_create("salts","Standard","BytesType",
                "BytesType", salts_table, salts_columns) == -1) {
        goto done;
    }   

    return (E_OK);

done:
    qDebug() << "Error";
    return (E_ERR);
}

    int 
libcass_close ()
{
    cassandra.synchronizeSchemaVersions();
    return (E_OK);
}

    int 
libcass_login (const char *email, const char *password, 
               char *userdata)
{
    char digest[SHA512_DIGEST_LENGTH], saltarr[SALT_SIZE];
    QtCassandra::QCassandraValue temp, salt, stored_pwd;
    QtCassandra::QCassandraValue f, l, p, pc;
    QString user, strid, dummy; int i;

    cassandra[keyspace]["master"].clearCache();
    cassandra[keyspace]["users"].clearCache();
    if (!cassandra[keyspace]["master"].exists(QString(email))) {
        qDebug() << "Unknown User";
        return (E_ERR);
    }

    temp = cassandra[keyspace]["master"][email]["id"];
    stored_pwd = cassandra[keyspace]["master"][email]["password"];
    strid = temp.stringValue(); 
    QUuid id(strid);
    salt = cassandra[keyspace]["salts"][id]["salt"];

    dummy = salt.binaryValue(); 
    for (i = 0; i < SALT_SIZE; i++) {
        saltarr[i] = (dummy.at(i)).toAscii();
    }

    if (gen_password(password, digest, saltarr) != E_OK) {
        return (E_ERR);
    } 
    QtCassandra::QCassandraValue digest_value(QByteArray(digest, SHA512_DIGEST_LENGTH));

    f = cassandra[keyspace]["users"][id]["firstname"];
    l = cassandra[keyspace]["users"][id]["lastname"];
    p = cassandra[keyspace]["users"][id]["paid"];
    pc = cassandra[keyspace]["users"][id]["pc"];

    if (digest_value == stored_pwd) {
        mkjson_user(user, id, f, l, p, pc);
        for (int i = 0; i < user.size() ; i++) { 
            userdata[i] = (user.at(i)).toAscii();
        }
        qDebug() << "User:" << user;
        return (E_OK);
    } else {
        return (E_ERR);
    }
}

    int 
libcass_signup (const char *email, const char * passwd,
                const char *first, const char *last,
                bool is_employer)
{
    QSharedPointer<QtCassandra::QCassandraRow>  master_row;
    QSharedPointer<QtCassandra::QCassandraRow>  user_row;
    QSharedPointer<QtCassandra::QCassandraRow>  salt_row;
    QSharedPointer<QtCassandra::QCassandraCell> cell;
    QtCassandra::QCassandraValue e, p, fn, ln, is_emp, salt_val; 
    QByteArray row_key, temp;
    QUuid uuid;
    char *salt;
    char *digest;


    if (email == NULL || passwd == NULL || first == NULL 
            || last == NULL) {
        qDebug() << "param problem";
        return (E_PARAM_PROBLEM);
    }

    if (validate_email(email) == false) {
        return(E_ERR);
    }

    cassandra[keyspace]["master"].clearCache();
    cassandra[keyspace]["users"].clearCache();
    //If entry exists then no signup for you 
    if (cassandra[keyspace]["master"].exists(QString(email))) {
        qDebug() << "Exsisting user " ;
        return (E_EXISTS);
    }

    /* Generate the uuid. XXX FIXME
       There is a possiblity of duplicate UUIDs. */
    uuid = uuid.createUuid();

    master_row = master_table->row(email);
    row_key = master_row->rowKey();

    user_row = user_table->row(uuid);
    salt_row = salts_table->row(uuid);

    salt = (char *) memory_alloc (SALT_SIZE);

    digest = (char *) memory_alloc (SHA512_DIGEST_LENGTH);

    if (gen_password(passwd, digest, salt) != E_OK) {
        free(salt); 
        free(digest); 
        return (E_ERR);
    }
    p.setBinaryValue(QByteArray(digest, SHA512_DIGEST_LENGTH));
    fn.setStringValue(QString(first));
    ln.setStringValue(QString(last));
    is_emp.setBoolValue(is_employer);

    // Convert from QString to const char*

    temp = uuid.toByteArray(); 

    // Store the master table entry 
    cassandra[keyspace]["master"][row_key]["password"] = p;
    cassandra[keyspace]["master"][row_key]["is_emplyer"] = is_emp;

    QtCassandra::QCassandraValue id(temp);
    qDebug() << "id"<< id.stringValue();

    cassandra[keyspace]["master"][row_key]["id"] = id;


    // Store the user table entry 
    row_key = user_row->rowKey();
    cassandra[keyspace]["users"][row_key]["firstname"] = fn;
    cassandra[keyspace]["users"][row_key]["lastname"] = ln;

    row_key = salt_row->rowKey();
    salt_val.setBinaryValue(QByteArray(salt, SALT_SIZE));
    cassandra[keyspace]["salts"][row_key]["salt"] = salt_val;
    free(salt); 
    free(digest); 

    return (E_OK);
}

int
libcass_getresumeinfo (char *uuid, char *resumedata)
{
    QtCassandra::QCassandraValue v;
    QString resume;

    if (uuid == NULL || resumedata == NULL) { 
        return (E_ERR);
    }

    QUuid id(uuid);

    cassandra[keyspace]["resumes"].clearCache();

    if (!cassandra[keyspace]["resumes"].exists(id)) { 
        qDebug() << "Unknown UUID" << id;
        return(E_ERR);
    } 

    v = cassandra[keyspace]["resumes"][id]["resume_info"];
    mkjson_resume(resume, id, v);
    for (int i = 0; i < resume.size() ; i++) { 
        resumedata[i] = (resume.at(i)).toAscii();
    }
    return (E_OK);
}


int
libcass_getvideoinfo (char *uuid, char *videodata)
{
    QtCassandra::QCassandraValue v;
    QString video;

    if (uuid == NULL) { 
        return (E_ERR);
    }

    QUuid id(uuid);

    cassandra[keyspace]["videos"].clearCache();

    if (!cassandra[keyspace]["videos"].exists(id)) { 
        qDebug() << "Unknown UUID" << id;
        return(E_ERR);
    } 

    v = cassandra[keyspace]["videos"][id]["video_info"];
    mkjson_video(video, id, v);
    
    for (int i = 0; i < video.size() ; i++) { 
        videodata[i] = (video.at(i)).toAscii();
    }
    return (E_OK);
}

int
libcass_setresinfo (char* resume_add, char *uuid) 
{
    QtCassandra::QCassandraValue v;
    QUuid id(uuid);
    QString str, val, resume;
    

    if (uuid == NULL || resume_add == NULL) { 
        return (E_ERR);
    }
  
    val = QString(resume_add);
   
    cassandra[keyspace]["resumes"].clearCache();
    cassandra[keyspace]["users"].clearCache();

    if (!cassandra[keyspace]["users"].exists(id)) { 
        qDebug() << "Unknown UUID" << id;
        return(E_ERR);
    } 

    v = cassandra[keyspace]["resumes"][id]["resume_info"];

    str = QString(v.binaryValue());

    if (str.contains(val)) {  
        qDebug() << "Resume Exists" ;
        return (E_EXISTS);
    } 

    QVariantMap temp;
    QVariantList list;
    QJson::Serializer serial;
    QByteArray json;
    QJson::Parser parser;

    temp = parser.parse(v.binaryValue()).toMap();
    temp.insert("resume_info", resume_add);
    list << temp;
    
    json = serial.serialize(list);
    
    v.setBinaryValue(json);
     
    cassandra[keyspace]["resumes"][id]["resume_info"] = v;
    mkjson_resume(resume, id, v);

    return (E_OK);
}


int
libcass_setvideoinfo (char* video_add, char *uuid) 
{
    QtCassandra::QCassandraValue v;
    QUuid id(uuid);
    QString str, val, video;
    

    if (uuid == NULL) { 
        return (E_ERR);
    }

    val = QString(video_add);

    cassandra[keyspace]["videos"].clearCache();
    cassandra[keyspace]["users"].clearCache();

    if (!cassandra[keyspace]["users"].exists(id)) { 
        qDebug() << "Unknown UUID" << id;
        return(E_ERR);
    } 

    v = cassandra[keyspace]["videos"][id]["video_info"];

    str = QString(v.binaryValue());

    if (str.contains(video_add)) {  
        qDebug() << "Video Exists" ;
        return (E_EXISTS);
    } 

    QVariantMap temp;
    QVariantList list;
    QJson::Serializer serial;
    QByteArray json;
    QJson::Parser parser;
    bool ok;

    temp = parser.parse(v.binaryValue(), &ok).toMap();
    if (!ok) { 
        qFatal("Parse error");
        assert(0);
    }
    temp.insert("video_info", video_add);
    list << temp;
    
    json = serial.serialize(list);
    
    v.setBinaryValue(json);
     
    cassandra[keyspace]["videos"][id]["video_info"] = v;
    mkjson_video(video, id, v);

    return (E_OK);
}

int
libcass_getuserinfo (char *uuid, char *userdata) 
{
    QtCassandra::QCassandraValue f, l, p, pc;
    QUuid id(uuid);
    QString user;

    cassandra[keyspace]["users"].clearCache();

    if (!cassandra[keyspace]["users"].exists(id)) { 
        qDebug() << "Unknown UUID" << id;
        return(E_ERR);
    } 

    f = cassandra[keyspace]["users"][id]["firstname"];
    l = cassandra[keyspace]["users"][id]["lastname"];
    p = cassandra[keyspace]["users"][id]["paid"];
    pc = cassandra[keyspace]["users"][id]["pc"];

    mkjson_user(user, id, f, l, p, pc);
    for (int i = 0; i < user.size() ; i++) { 
        userdata[i] = (user.at(i)).toAscii();
    }
    qDebug() << "User:" << user;
    return (E_OK);
}
