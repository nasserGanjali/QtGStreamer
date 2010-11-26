/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef QGLIB_VALUE_H
#define QGLIB_VALUE_H

#include "global.h"
#include "type.h"
#include "refpointer.h"
#include <boost/mpl/if.hpp>
#include <boost/type_traits.hpp>
#include <stdexcept>
#include <QtCore/QString>
#include <QtCore/QDebug>

namespace QGlib {

struct ValueVTable
{
    typedef void (*SetFunction)(ValueBase & value, const void *data);
    typedef void (*GetFunction)(const ValueBase & value, void *data);

    inline ValueVTable() : set(NULL), get(NULL) {}
    inline ValueVTable(SetFunction s, GetFunction g) : set(s), get(g) {}

    SetFunction set;
    GetFunction get;
};


/*! \headerfile value.h <QGlib/Value>
 * \brief Common base class for Value and SharedValue, wrappers for GValue
 */
class ValueBase
{
public:
    /*! \headerfile value.h <QGlib/Value>
     * Exception thrown from ValueBase::set and ValueBase::get when the ValueBase
     * instance on which they are being called has not been initialized. You can
     * check if a ValueBase instance has been initialized with ValueBase::isValid()
     * and if is actually a Value instance, you can initialize it by calling Value::init().
     */
    class InvalidValueException : public std::logic_error
    {
    public:
        inline InvalidValueException()
            : std::logic_error("This ValueBase instance has not been initialized") {}
    };

    /*! \headerfile value.h <QGlib/Value>
     * Exception thrown from ValueBase::set and ValueBase::get when the ValueBase
     * instance on which they are being called has been initialized to hold a different
     * type than the one that is being accessed.
     */
    class InvalidTypeException : public std::logic_error
    {
    public:
        inline InvalidTypeException()
            : std::logic_error("This ValueBase instance has been initialized to hold a different "
                                 "type of data than the one that you are trying to access") {}
    };

    /*! \returns whether this ValueBase instance wraps a valid GValue instance or not */
    bool isValid() const;

    /*! Clears the current value in this GValue instance and resets it to the
     * default value (as if the GValue had just been initialized). */
    void reset();

    /*! Retrieves the value stored in this GValue instance, as the specified type T.
     * The bindings take care of calling the appropriate g_value_get_* method depending
     * on the type T. Note though that this is only meant to be used with the types of
     * the bindings, not their C types. This means that if for example the GValue has
     * been initialized to hold a GstObject pointer, you can use:
     * \code
     * QGst::ObjectPtr object = value.get<QGst::ObjectPtr>();
     * \endcode
     * but \em not this:
     * \code
     * GstObject *object = value.get<GstObject*>(); //will cause compilation error
     * \endcode
     * \throws InvalidValueException if this instance is invalid (see isValid())
     * \throws InvalidTypeException if the type T does not match the type stored
     * in this GValue instance.
     */
    template <typename T> T get() const;

    /*! Sets this GValue instance to hold the specified \a data.
     * As with get(), the bindings take care of calling the appropriate g_value_set_*
     * method depending on the type T, but note that this is only meant to be used
     * with the types of the bindings.
     * \throws InvalidValueException if this instance is invalid (see isValid())
     * \throws InvalidTypeException if the type T does not match the type that this
     * GValue instance has been initialized to hold.
     * \sa get()
     */
    template <typename T> void set(const T & data);

    /*! \returns the type that this GValue instance has been initialized to hold */
    Type type() const;

    /*! \returns whether it is possible to transform this GValue to a GValue of another type. */
    bool canTransformTo(Type type) const;

    /*! Transforms the current value into a value of the specified \a type, if possible.
     * Possible transformations are, for example, int to float, int to string, etc... */
    Value transformTo(Type type) const;

    inline operator GValue*() { return m_value; }
    inline operator const GValue*() const { return m_value; }

    /*! \internal */
    static void registerValueVTable(Type type, const ValueVTable & vtable);

protected:
    template <typename T>
    friend struct ValueImpl;

    /*! \internal */
    void getData(Type dataType, void *data) const;
    /*! \internal */
    void setData(Type dataType, const void *data);


    ValueBase(GValue *val);
    virtual ~ValueBase();
    Q_DISABLE_COPY(ValueBase)

    GValue *m_value;
};

/*! \headerfile value.h <QGlib/Value>
 * \brief Wrapper class for GValue
 *
 * This class serves as a wrapper for GValue. GValue is a data type that can hold different
 * types of values inside, like a QVariant.
 * To set a value, you must first initialize this Value using one of the init() methods
 * (preferably the template one) in order to tell it what kind of value it is going to hold.
 * Once initialized to hold a specific type, you can use the set() and get() methods to set and
 * get values of this type only. Attempting to use another type will assert. To change the type
 * that this value holds, you can call the init() method again at any time. In this case, though,
 * any held value will be lost.
 * \note Unlike SharedValue, this class always keeps a private GValue internally which is
 * allocated in the constructors and freed from the destructor.
 * \sa SharedValue
 */
class Value : public ValueBase
{
public:
    Value();
    explicit Value(const GValue *gvalue);
    template <typename T>
    inline Value(const T & data);
    Value(const SharedValue & other);
    Value(const Value & other);
    virtual ~Value();

    Value & operator=(const SharedValue & other);
    Value & operator=(const Value & other);

    void init(Type type);

    template <typename T>
    inline void init();
};

/*! \headerfile value.h <QGlib/Value>
 * \brief Wrapper class for shared GValue instances
 *
 * This class serves as a wrapper for shared GValue instances. Some functions in the GStreamer
 * API return a pointer to some internal GValue and expect you to change this internal instance,
 * not copy it and re-set it using some setter function (like all normal object-oriented APIs do),
 * so it is necessary to have way of accessing those instances. This class wraps a GValue without
 * copying it and without freeing it from the destructor, unlike Value, which always keeps a
 * GValue instance for itself.
 * \sa Value
 */
class SharedValue : public ValueBase
{
public:
    explicit SharedValue(GValue *gvalue);
    SharedValue(const SharedValue & other);
    virtual ~SharedValue();
    SharedValue & operator=(const SharedValue & other);
};


/*! This class provides the implementation for the ValueBase::get() and ValueBase::set() methods.
 * If you want to provide support for a custom type, you should provide a template specialization
 * of this class that sets and gets data to the underlying GValue for your custom type.
 * The default implementation expects T to be an enum.
 * \sa QGLIB_REGISTER_VALUEIMPL
 */
template <typename T>
struct ValueImpl
{
    static inline T get(const ValueBase & value);
    static inline void set(ValueBase & value, const T & data);
};

// -- template implementations --

template <typename T>
T ValueBase::get() const
{
    try {
        return ValueImpl<T>::get(*this);
    } catch (const std::exception & e) {
        qWarning() << "QGlib::ValueBase::get:" << e.what();
        return T();
    }
}

template <typename T>
void ValueBase::set(const T & data)
{
    try {
        ValueImpl<T>::set(*this, data);
    } catch (const std::exception & e) {
        qWarning() << "QGlib::ValueBase::set:" << e.what();
    }
}


template <typename T>
inline Value::Value(const T & data)
    : ValueBase(NULL)
{
    init<T>();
    set(data);
}

template <typename T>
inline void Value::init()
{
    init(GetType<T>());
}

// -- default ValueImpl implementation --

template <typename T>
inline T ValueImpl<T>::get(const ValueBase & value)
{
    //Use int for enums, T for everything else
    typename boost::mpl::if_<
        boost::is_enum<T>,
        int, T
    >::type result;

    value.getData(GetType<T>(), &result);
    return static_cast<T>(result);
}

template <typename T>
inline void ValueImpl<T>::set(ValueBase & value, const T & data)
{
    //Use const int for enums, const T for everything else
    typename boost::mpl::if_<
        boost::is_enum<T>,
        const int, const T &
    >::type dataRef = data;

    value.setData(GetType<T>(), &dataRef);
}

// -- ValueImpl specialization for QFlags --

template <class T>
struct ValueImpl< QFlags<T> >
{
    static inline QFlags<T> get(const ValueBase & value)
    {
        uint flags;
        value.getData(GetType< QFlags<T> >(), &flags);
        return QFlags<T>(QFlag(flags));
    }

    static inline void set(ValueBase & value, const QFlags<T> & data)
    {
        uint flags = data;
        value.setData(GetType< QFlags<T> >(), &flags);
    }
};

// -- ValueImpl specialization for RefPointer --

template <class T>
struct ValueImpl< RefPointer<T> >
{
    static inline RefPointer<T> get(const ValueBase & value)
    {
        typename T::CType *gobj;
        value.getData(GetType<T>(), &gobj);
        return RefPointer<T>::wrap(gobj);
    }

    static inline void set(ValueBase & value, const RefPointer<T> & data)
    {
        typename T::CType *gobj = static_cast<typename T::CType*>(data);
        value.setData(GetType<T>(), &gobj);
    }
};

// -- ValueImpl specialization for string literals --

template <int N>
struct ValueImpl<char[N]>
{
    //No get method, obviously.

    static inline void set(ValueBase & value, const char (&data)[N])
    {
        QByteArray str = QByteArray::fromRawData(data, N);
        value.setData(Type::String, &str);
    }
};

// -- ValueImpl specialization for const char* --

template <>
struct ValueImpl<const char*>
{
    //No get method, obviously.

    static inline void set(ValueBase & value, const char *data)
    {
        QByteArray str = QByteArray::fromRawData(data, qstrlen(data));
        value.setData(Type::String, &str);
    }
};

// -- ValueImpl specialization for QString --

template <>
struct ValueImpl<QString>
{
    static inline QString get(const ValueBase & value)
    {
        QByteArray str;
        value.getData(Type::String, &str);
        return QString::fromUtf8(str);
    }

    static inline void set(ValueBase & value, const QString & data)
    {
        QByteArray str = data.toUtf8();
        value.setData(Type::String, &str);
    }
};

} //namespace QGlib

/*! \relates QGlib::ValueBase */
QDebug & operator<<(QDebug debug, const QGlib::ValueBase & value);

QGLIB_REGISTER_TYPE(QGlib::ValueBase) //codegen: GType=G_TYPE_VALUE
QGLIB_REGISTER_TYPE(QGlib::Value)
QGLIB_REGISTER_TYPE(QGlib::SharedValue) //codegen: GType=G_TYPE_VALUE

#endif
