# Compile
`make`

# Execute

## Server
`./server_ex <IP_ADRESSE>`

## Client 
`./client_ex <IP_ADRESSE> <name>`

# Client Functionalities

## Send message to groupe:
`#Send #Group #NameOfGroup message`

## Send a private message :
`#Send #NameOfPerson message`

## Send a message to everyone:
`#Send #all message`

## Create a Group
`#Create #NameOfGroup person1 person2 person3`

comment : the client doesn't need to add his name, he will be automaticaly added to the group

## Add someone to an existing group
`#Add #NameOfGroup person`

## Remove someone to an existing group
`#Remove #NameOfGroup person`

## Send a file to someone
`#Send #file #NameOfPerson NameOfFile`  

comment : the person has to be connected

## See the members of a given group
`#Get #memberGroup #NameOfgroup`

## See which group the current client is belonging
`#Get #myGroups`

# Server Functionalities
To close the server, enter anything in the keyboard and it will close all the clients connections and save automatically the data.