//--------------------------------------------------------------------------------------------------
/**
 * Implementation of the Component Instance class.
 *
 * Copyright (C) 2013 Sierra Wireless Inc., all rights reserved.
 */
//--------------------------------------------------------------------------------------------------

#include "LegatoObjectModel.h"


namespace legato
{


//--------------------------------------------------------------------------------------------------
/**
 * Constructor
 */
//--------------------------------------------------------------------------------------------------
ComponentInstance::ComponentInstance
(
    Component& component
)
//--------------------------------------------------------------------------------------------------
:   m_Name(component.Name()),
    m_Component(component)
//--------------------------------------------------------------------------------------------------
{
}


//--------------------------------------------------------------------------------------------------
/**
 * Copy constructor
 **/
//--------------------------------------------------------------------------------------------------
ComponentInstance::ComponentInstance
(
    const ComponentInstance& source
)
//--------------------------------------------------------------------------------------------------
:   m_Name(source.m_Name),
    m_Component(source.m_Component)
//--------------------------------------------------------------------------------------------------
{
}



//--------------------------------------------------------------------------------------------------
/**
 * Move constructor
 **/
//--------------------------------------------------------------------------------------------------
ComponentInstance::ComponentInstance
(
    ComponentInstance&& rvalue
)
//--------------------------------------------------------------------------------------------------
:   m_Name(std::move(rvalue.m_Name)),
    m_Component(rvalue.m_Component)
//--------------------------------------------------------------------------------------------------
{
}



//--------------------------------------------------------------------------------------------------
/**
 * Destructor
 **/
//--------------------------------------------------------------------------------------------------
ComponentInstance::~ComponentInstance()
{
}



//--------------------------------------------------------------------------------------------------
/**
 * Move assignment operator (=)
 **/
//--------------------------------------------------------------------------------------------------
ComponentInstance& ComponentInstance::operator =
(
    ComponentInstance&& rvalue
)
//--------------------------------------------------------------------------------------------------
{
    if (&rvalue != this)
    {
        m_Name = std::move(rvalue.m_Name);
        m_Component = std::move(rvalue.m_Component);
    }

    return *this;
}




//--------------------------------------------------------------------------------------------------
/**
 * Get the name of the component instance.
 */
//--------------------------------------------------------------------------------------------------
const std::string& ComponentInstance::Name() const
{
    return m_Name;
}



//--------------------------------------------------------------------------------------------------
/**
 * Set the name of the component instance.
 */
//--------------------------------------------------------------------------------------------------
void ComponentInstance::Name(const std::string& name)
{
    m_Name = name;
}



}