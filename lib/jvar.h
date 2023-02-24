
#ifndef jvar_h
#define jvar_h
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <ostream>
#include <algorithm>

namespace picojson {
    class value;
}  

enum jvar_types{
    JBOOL,
    JNUMBER,
    JLONGINTEGER,
    JSTRING,
    JOBJECT,
    JARRAY,
    JNULL
};

class Mnull{
};
#define MNULL Mnull()
#define jv (jvar)
#define ZERO (size_t)0
#define lInt long long int
#define jcmp [](jvar& a, jvar& b)

class jvar;
/*JItem, used to initialize a J Object members */
typedef std::pair<std::string,jvar> ji;
/*shorthand for std::vector<jvar> ( jvar array )*/
typedef std::vector<jvar> ja;
/*shorthand for std::map<std::string,jvar> (jvar object)*/
typedef std::map<std::string,jvar> jo;

struct JvarIterableType {
    jvar* root;
    jvar* p;
    std::map<std::string, jvar>::iterator it;
    std::map<std::string, jvar>::iterator itend;
    size_t i;
    JvarIterableType(jvar* parg);
    JvarIterableType(jvar* parg, bool end);
    bool operator!=(JvarIterableType rhs);
    jvar& operator*() {return *p;}
    void operator++();
};

class jvar
{
private:
    jvar_types type=JNULL;
    char *buffer;

    void deleteCurrent();
    void copyInit(const jvar &source);
    void replaceAllInternal( std::string &s, const std::string &search, const std::string &replace );
    static jvar parsePico(picojson::value obj);
public:
    jvar_types getType();
    char * getBuffer();
    ~jvar();
    template <class TType> void startBuffer( TType val);
    //constructors 
    jvar(bool val);
    jvar(char val);
    jvar(int val);
    jvar(long int val);
    jvar(long long int val);
    jvar(size_t val);
    jvar(float val);
    jvar(double val);
    jvar(Mnull val);
    jvar();
    jvar(const std::string &valarg);
    jvar(const std::vector<jvar> &valarg);
    jvar(const std::map<std::string,jvar> &valarg);
    jvar(const char val[]);
    jvar(const jvar &source);
    jvar(jvar &&source) noexcept;

    //operators that modify original
    jvar& operator= (jvar&& other) noexcept;
    jvar& operator=(const jvar& other);
    jvar& operator+=(const jvar& other);
    jvar& operator-=(const jvar& other);
    jvar& operator*=(const jvar& other);
    jvar& operator/=(const jvar& other);

    //converters
    static jvar parseJson(std::string json);
    /*returns the value as a boolean if possible, or false*/
    bool asBoolean();
    /*returns the value as a double if possible, or 0.0*/
    double asDouble();
    /*returns the value as a long long int if possible, or 0*/
    long long int asInteger();
    /*serialize to JSON*/
    std::string toJson();
    /*serialize to XML*/
    std::string toXml();
    /*serialize to XML*/
    std::string toXmlPeekParameters();
    /* Make a Pretty Json for debugging */
    std::string prettyString();
    /*If you are returning a string, returns the original string, not the serializable one*/
    std::string asString();

    operator short int() const { return ((jvar)(*this)).asInteger(); }
    operator long long int() const { return ((jvar)(*this)).asInteger(); }
    operator size_t() const { return ((jvar)(*this)).asInteger(); }
    operator long int() const { return ((jvar)(*this)).asInteger(); }
    operator int() const { return ((jvar)(*this)).asInteger(); }
    operator float() const { return ((jvar)(*this)).asDouble(); }
    operator double() const { return ((jvar)(*this)).asDouble(); }
    operator bool() const { return ((jvar)(*this)).asBoolean(); }
    operator std::string() const { return ((jvar)(*this)).asString(); }

    bool isNumeric();
    bool isInteger();
    bool isRealNumber();
    bool isString();
    bool isArray();
    bool isObject();
    bool isNull();

    //access operators
    jvar &operator[](std::string pos) ;
    jvar &operator[](const char pos[]) ;
    jvar &operator[](size_t index) ;
    /*only do something if jvar is a string */
    size_t size();
    size_t memoryFootPrint();

    bool isEmpty();
    jvar entries();
    jvar keys();
    jvar values();
    
    /*string and object indexOf*/
    size_t indexOf(std::string str, size_t start=0);
    size_t lastIndexOf(std::string str, size_t start=-1);

    /*string only functions*/
    jvar replaceAll( const std::string &search, const std::string &replace );
    /*string only functions*/
    jvar replace( const std::string &search, const std::string &replace );
    /*string only functions*/
    jvar replaceRegex( const std::string &regex, const std::string &replace );
    /*string only functions*/
    jvar substr(size_t start, size_t end);
    /*string only functions, from @start to end of string*/
    jvar substr(size_t start);
    /*string only functions, from @start to end of string*/
    jvar charAt(size_t pos); 

    jvar trim();
    jvar toLowerCase();
    jvar toUpperCase();
    jvar toCapitalized();

    /*string only function, tranform string into array separated by @separator*/
    jvar split(std::string separator);
    /*array only function, tranform array into string separated by @separator*/
    jvar join(std::string separator);
    /*array only function, start an jvar array with N null elements*/
    jvar &allocateArrayWithNElements(size_t n);
    
    /*object only*/
    size_t eraseItem(std::string key);

    /*array only sort*/
    void sortAsc();
    void sortDesc();
    void sort(std::function<bool(jvar&, jvar&)> comparatorFunction);
    
    //Equality operators
    friend bool operator==(const jvar &left, const jvar &right);
    friend bool operator!=(const jvar &left, const jvar &right);
    friend bool operator>(const jvar &left, const jvar &right);
    friend bool operator>=(const jvar &left, const jvar &right);
    friend bool operator<(const jvar &left, const jvar &right);
    friend bool operator<=(const jvar &left, const jvar &right);
    //friend char operator<=>(const jvar &left, const jvar &right);

    //Math Operators
    /*applies to all numerics and to every member of arrays and objects*/
    friend jvar operator%(jvar left, const jvar &right);
    /*applies to all numerics and to every member of arrays and objects*/
    friend jvar operator/(jvar left, const jvar &right);
    /*applies to all numerics and to every member of arrays and objects*/
    friend jvar operator*(jvar left, const jvar &right);
    /*applies to all numerics and to every member of arrays and objects*/
    friend jvar operator+(jvar left, const jvar &right);
    /*applies to all numerics and to every member of arrays and objects*/
    friend jvar operator-(jvar left, const jvar &right);
    /*creates a jvar pair (jitem) std::pair<std::string,jvar> using left as key and right as value */
    friend ji operator<<(const char left[], const jvar &right);
    friend std::ostream& operator<<(std::ostream& os, const jvar& right);
    friend jvar& operator<<(jvar &left, const jvar &right);
    
    /*Convert Operators*/
    friend jvar operator+(jvar left, const double &right);
    friend jvar operator+(jvar left, const int &right);
    friend jvar operator+(jvar left, const long int &right);
    friend jvar operator+(jvar left, const long long int &right);
    friend jvar operator+(jvar left, const short int &right);
    friend jvar operator+(jvar left, const float &right);
    friend jvar operator+(jvar left, const char &right);
    friend jvar operator+(jvar left, const char right[]);
    friend jvar operator+(jvar left, const std::string right);
    friend jvar operator+(double left, const jvar &right);
    friend jvar operator+(float left, const jvar &right);
    friend jvar operator+(char left, const jvar &right);
    friend jvar operator+(short int left, const jvar &right);
    friend jvar operator+(int left, const jvar &right);
    friend jvar operator+(long int left, const jvar &right);
    friend jvar operator+(long long int left, const jvar &right);
    friend jvar operator+(const char left[], const jvar &right);
    friend jvar operator+(const std::string left, const jvar &right);

    friend jvar operator-(jvar left, const double &right);
    friend jvar operator-(jvar left, const int &right);
    friend jvar operator-(jvar left, const long int &right);
    friend jvar operator-(jvar left, const long long int &right);
    friend jvar operator-(jvar left, const short int &right);
    friend jvar operator-(jvar left, const float &right);
    friend jvar operator-(jvar left, const char &right);
    friend jvar operator-(double left, const jvar &right);
    friend jvar operator-(float left, const jvar &right);
    friend jvar operator-(char left, const jvar &right);
    friend jvar operator-(short int left, const jvar &right);
    friend jvar operator-(int left, const jvar &right);
    friend jvar operator-(long int left, const jvar &right);
    friend jvar operator-(long long int left, const jvar &right);

    
    friend jvar operator*(jvar left, const double &right);
    friend jvar operator*(jvar left, const int &right);
    friend jvar operator*(jvar left, const long int &right);
    friend jvar operator*(jvar left, const long long int &right);
    friend jvar operator*(jvar left, const short int &right);
    friend jvar operator*(jvar left, const float &right);
    friend jvar operator*(jvar left, const char &right);
    friend jvar operator*(double left, const jvar &right);
    friend jvar operator*(float left, const jvar &right);
    friend jvar operator*(char left, const jvar &right);
    friend jvar operator*(short int left, const jvar &right);
    friend jvar operator*(int left, const jvar &right);
    friend jvar operator*(long int left, const jvar &right);
    friend jvar operator*(long long int left, const jvar &right);

    friend jvar operator/(jvar left, const double &right);
    friend jvar operator/(jvar left, const int &right);
    friend jvar operator/(jvar left, const long int &right);
    friend jvar operator/(jvar left, const long long int &right);
    friend jvar operator/(jvar left, const short int &right);
    friend jvar operator/(jvar left, const float &right);
    friend jvar operator/(jvar left, const char &right);
    friend jvar operator/(double left, const jvar &right);
    friend jvar operator/(float left, const jvar &right);
    friend jvar operator/(char left, const jvar &right);
    friend jvar operator/(short int left, const jvar &right);
    friend jvar operator/(int left, const jvar &right);
    friend jvar operator/(long int left, const jvar &right);
    friend jvar operator/(long long int left, const jvar &right);

    friend jvar operator%(jvar left, const int &right);
    friend jvar operator%(jvar left, const long int &right);
    friend jvar operator%(jvar left, const long long int &right);
    friend jvar operator%(jvar left, const short int &right);
    friend jvar operator%(jvar left, const char &right);
    friend jvar operator%(char left, const jvar &right);
    friend jvar operator%(short int left, const jvar &right);
    friend jvar operator%(int left, const jvar &right);
    friend jvar operator%(long int left, const jvar &right);
    friend jvar operator%(long long int left, const jvar &right);


    friend bool operator==(jvar left, const bool &right);
    friend bool operator==(jvar left, const double &right);
    friend bool operator==(jvar left, const int &right);
    friend bool operator==(jvar left, const long int &right);
    friend bool operator==(jvar left, const long long int &right);
    friend bool operator==(jvar left, const short int &right);
    friend bool operator==(jvar left, const float &right);
    friend bool operator==(jvar left, const char &right);
    friend bool operator==(jvar left, const char right[]);
    friend bool operator==(bool left, const jvar &right);
    friend bool operator==(double left, const jvar &right);
    friend bool operator==(float left, const jvar &right);
    friend bool operator==(char left, const jvar &right);
    friend bool operator==(short int left, const jvar &right);
    friend bool operator==(int left, const jvar &right);
    friend bool operator==(long int left, const jvar &right);
    friend bool operator==(long long int left, const jvar &right);
    friend bool operator==(const char left[], const jvar &right);

    friend bool operator!=(jvar left, const double &right);
    friend bool operator!=(jvar left, const int &right);
    friend bool operator!=(jvar left, const long int &right);
    friend bool operator!=(jvar left, const long long int &right);
    friend bool operator!=(jvar left, const short int &right);
    friend bool operator!=(jvar left, const float &right);
    friend bool operator!=(jvar left, const char &right);
    friend bool operator!=(jvar left, const char right[]);
    friend bool operator!=(double left, const jvar &right);
    friend bool operator!=(float left, const jvar &right);
    friend bool operator!=(char left, const jvar &right);
    friend bool operator!=(short int left, const jvar &right);
    friend bool operator!=(int left, const jvar &right);
    friend bool operator!=(long int left, const jvar &right);
    friend bool operator!=(long long int left, const jvar &right);
    friend bool operator!=(const char left[], const jvar &right);

    friend bool operator>(jvar left, const double &right);
    friend bool operator>(jvar left, const int &right);
    friend bool operator>(jvar left, const long int &right);
    friend bool operator>(jvar left, const long long int &right);
    friend bool operator>(jvar left, const short int &right);
    friend bool operator>(jvar left, const float &right);
    friend bool operator>(jvar left, const char &right);
    friend bool operator>(jvar left, const char right[]);
    friend bool operator>(double left, const jvar &right);
    friend bool operator>(float left, const jvar &right);
    friend bool operator>(char left, const jvar &right);
    friend bool operator>(short int left, const jvar &right);
    friend bool operator>(int left, const jvar &right);
    friend bool operator>(long int left, const jvar &right);
    friend bool operator>(long long int left, const jvar &right);
    friend bool operator>(const char left[], const jvar &right);

    friend bool operator>=(jvar left, const double &right);
    friend bool operator>=(jvar left, const int &right);
    friend bool operator>=(jvar left, const long int &right);
    friend bool operator>=(jvar left, const long long int &right);
    friend bool operator>=(jvar left, const short int &right);
    friend bool operator>=(jvar left, const float &right);
    friend bool operator>=(jvar left, const char &right);
    friend bool operator>=(jvar left, const char right[]);
    friend bool operator>=(double left, const jvar &right);
    friend bool operator>=(float left, const jvar &right);
    friend bool operator>=(char left, const jvar &right);
    friend bool operator>=(short int left, const jvar &right);
    friend bool operator>=(int left, const jvar &right);
    friend bool operator>=(long int left, const jvar &right);
    friend bool operator>=(long long int left, const jvar &right);
    friend bool operator>=(const char left[], const jvar &right);

    friend bool operator<(jvar left, const double &right);
    friend bool operator<(jvar left, const int &right);
    friend bool operator<(jvar left, const long int &right);
    friend bool operator<(jvar left, const long long int &right);
    friend bool operator<(jvar left, const short int &right);
    friend bool operator<(jvar left, const float &right);
    friend bool operator<(jvar left, const char &right);
    friend bool operator<(jvar left, const char right[]);
    friend bool operator<(double left, const jvar &right);
    friend bool operator<(float left, const jvar &right);
    friend bool operator<(char left, const jvar &right);
    friend bool operator<(short int left, const jvar &right);
    friend bool operator<(int left, const jvar &right);
    friend bool operator<(long int left, const jvar &right);
    friend bool operator<(long long int left, const jvar &right);
    friend bool operator<(const char left[], const jvar &right);

    friend bool operator<=(jvar left, const double &right);
    friend bool operator<=(jvar left, const int &right);
    friend bool operator<=(jvar left, const long int &right);
    friend bool operator<=(jvar left, const long long int &right);
    friend bool operator<=(jvar left, const short int &right);
    friend bool operator<=(jvar left, const float &right);
    friend bool operator<=(jvar left, const char &right);
    friend bool operator<=(jvar left, const char right[]);
    friend bool operator<=(double left, const jvar &right);
    friend bool operator<=(float left, const jvar &right);
    friend bool operator<=(char left, const jvar &right);
    friend bool operator<=(short int left, const jvar &right);
    friend bool operator<=(int left, const jvar &right);
    friend bool operator<=(long int left, const jvar &right);
    friend bool operator<=(long long int left, const jvar &right);
    friend bool operator<=(const char left[], const jvar &right);

    jvar& operator+=(const double &right);
    jvar& operator+=(const int &right);
    jvar& operator+=(const long int &right);
    jvar& operator+=(const long long int &right);
    jvar& operator+=(const short int &right);
    jvar& operator+=(const float &right);
    jvar& operator+=(const char &right);
    jvar& operator+=(const char right[]);  
    jvar& operator+=(const std::string right);  

    jvar& operator-=(const double &right);
    jvar& operator-=(const int &right);
    jvar& operator-=(const long int &right);
    jvar& operator-=(const long long int &right);
    jvar& operator-=(const short int &right);
    jvar& operator-=(const float &right);
    jvar& operator-=(const char &right); 

    jvar& operator*=(const double &right);
    jvar& operator*=(const int &right);
    jvar& operator*=(const long int &right);
    jvar& operator*=(const long long int &right);
    jvar& operator*=(const short int &right);
    jvar& operator*=(const float &right);
    jvar& operator*=(const char &right);

    jvar& operator/=(const double &right);
    jvar& operator/=(const int &right);
    jvar& operator/=(const long int &right);
    jvar& operator/=(const long long int &right);
    jvar& operator/=(const short int &right);
    jvar& operator/=(const float &right);
    jvar& operator/=(const char &right);


    JvarIterableType begin() {
        return JvarIterableType(this);
    }
    JvarIterableType end() {
        return JvarIterableType(this,true);
    }

};



#endif