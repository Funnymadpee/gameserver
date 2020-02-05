#pragma once

#include <map>
#include <list>
#include <memory>
#include <algorithm>
#include <queue>

#include "component.h"
#include "entity_system.h"

class IEntity :public IComponent
{
public:
    virtual ~IEntity() = default;
    void ComponentBackToPool() override;

    template <class T, typename... TArgs>
    T* AddComponent(TArgs... args);

    template<class T>
    T* GetComponent();

    template<class T>
    void RemoveComponent();

    void RemoveComponent(IComponent* pObj);

protected:
    // <type hash code , IComponent>
    std::map<uint64, IComponent*> _components;
};

template <class T, typename... TArgs>
inline T* IEntity::AddComponent(TArgs... args)
{
    auto pSystemManager = GetSystemManager();
    T* pComponent = pSystemManager->GetEntitySystem()->AddComponent<T>(std::forward<TArgs>(args)...);
    pComponent->SetParent(this);
    _components.insert(std::make_pair(pComponent->GetTypeHashCode(), pComponent));
    return pComponent;
}

template<class T>
T* IEntity::GetComponent()
{
    const auto typeHashCode = typeid(T).hash_code();
    const auto iter = _components.find(typeHashCode);
    if (iter == _components.end())
        return nullptr;

    return dynamic_cast<T*>(iter->second);
}

template<class T>
void IEntity::RemoveComponent()
{
    // ��ɾ�������������
    const auto typeHashCode = typeid(T).hash_code();
    const auto iter = _components.find(typeHashCode);
    if (iter == _components.end())
    {
        LOG_ERROR("Entity RemoveComponent error. not find. className:" << typeid(T).name());
        return;
    }

    auto pComponent = iter->second;
    RemoveComponent(pComponent);
}

template<class T>
class Entity :public IEntity
{
public:
    const char* GetTypeName() override;
    uint64 GetTypeHashCode() override;
};

template <class T>
const char* Entity<T>::GetTypeName()
{
    return typeid(T).name();
}

template <class T>
uint64 Entity<T>::GetTypeHashCode()
{
    return typeid(T).hash_code();
}