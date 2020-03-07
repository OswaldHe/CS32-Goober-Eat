#ifndef EXPANDABLE_H
#define EXPANDABLE_H
#include <list>
// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    struct Pair{
        KeyType key;
        ValueType value;
    };
    int numBucket;
    std::list<Pair>* m_list;
    double maxLoadFactor;
    int m_size;
    
    unsigned int getBucketNumber(const KeyType& key) const{
        unsigned int hash(const KeyType& key);
        return hash(key) % numBucket;
    }
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    numBucket = 8;
    m_list = new std::list<Pair>[numBucket];
    maxLoadFactor = maximumLoadFactor;
    m_size = 0;
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    delete [] m_list;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    delete [] m_list;
    numBucket = 8;
    m_size = 0;
    m_list = new std::list<Pair>[numBucket];
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_size;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    if((double)m_size/numBucket>maxLoadFactor){
        numBucket*=2;
        std::list<Pair>* replace = new std::list<Pair>[numBucket];
        for(int i = 0; i < numBucket/2; i++){
            if(m_list[i].size()!=0){
                typename std::list<Pair>::iterator it = m_list[i].begin();
                typename std::list<Pair>::iterator temp;
                for(; it!=m_list[i].end();it++){
                    unsigned int index = getBucketNumber((*it).key);
                    temp = it;
                    it++;
                    replace[index].splice(replace[index].end(), m_list[i], temp);
                }
            }
        }
        delete [] m_list;
        m_list = replace;
    }
    
    unsigned int index = getBucketNumber(key);
    Pair p = {key, value};
    if(m_list[index].size()==0){
        m_list[index].push_back(p);
        m_size++;
    }else{
        bool isDuplicate = false;
        typename std::list<Pair>::iterator it = m_list[index].begin();
        for (; it!=m_list[index].end(); it++) {
            if((*it).key==p.key){
                isDuplicate = true;
                (*it).value = p.value;
                break;
            }
        }
        if(!isDuplicate){
            m_list[index].push_back(p);
            m_size++;
        }
    }
    
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    if(m_size==0) return nullptr;
    unsigned int index = getBucketNumber(key);
    if(m_list[index].size()==0)return nullptr;
    typename std::list<Pair>::iterator it = m_list[index].begin();
    for (; it!=m_list[index].end(); it++) {
        if((*it).key == key) return &((*it).value);
    }
    return nullptr;
}



#endif /* EXPANDABLE_H */
