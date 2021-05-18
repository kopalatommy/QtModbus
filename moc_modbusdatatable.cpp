/****************************************************************************
** Meta object code from reading C++ file 'modbusdatatable.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "modbusdatatable.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbusdatatable.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ModbusDatatable_t {
    QByteArrayData data[6];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ModbusDatatable_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ModbusDatatable_t qt_meta_stringdata_ModbusDatatable = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ModbusDatatable"
QT_MOC_LITERAL(1, 16, 17), // "AlteredCoilsTable"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 21), // "AlteredDiscreteInputs"
QT_MOC_LITERAL(4, 57, 21), // "AlteredInputRegisters"
QT_MOC_LITERAL(5, 79, 23) // "AlteredHoldingRegisters"

    },
    "ModbusDatatable\0AlteredCoilsTable\0\0"
    "AlteredDiscreteInputs\0AlteredInputRegisters\0"
    "AlteredHoldingRegisters"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ModbusDatatable[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,
       4,    0,   36,    2, 0x06 /* Public */,
       5,    0,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ModbusDatatable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModbusDatatable *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->AlteredCoilsTable(); break;
        case 1: _t->AlteredDiscreteInputs(); break;
        case 2: _t->AlteredInputRegisters(); break;
        case 3: _t->AlteredHoldingRegisters(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ModbusDatatable::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModbusDatatable::AlteredCoilsTable)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ModbusDatatable::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModbusDatatable::AlteredDiscreteInputs)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ModbusDatatable::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModbusDatatable::AlteredInputRegisters)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ModbusDatatable::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ModbusDatatable::AlteredHoldingRegisters)) {
                *result = 3;
                return;
            }
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject ModbusDatatable::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ModbusDatatable.data,
    qt_meta_data_ModbusDatatable,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ModbusDatatable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusDatatable::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ModbusDatatable.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModbusDatatable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ModbusDatatable::AlteredCoilsTable()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ModbusDatatable::AlteredDiscreteInputs()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ModbusDatatable::AlteredInputRegisters()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ModbusDatatable::AlteredHoldingRegisters()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
