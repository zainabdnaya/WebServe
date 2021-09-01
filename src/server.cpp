/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdnaya <zdnaya@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/10 14:27:10 by zainabdnaya       #+#    #+#             */
/*   Updated: 2021/09/01 15:42:15 by zdnaya           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"
#include <string>
#include <dirent.h>

int check_dir(std::string dir, std::string str)
{
    DIR *dirp;
    struct dirent *dp;
    dirp = opendir(dir.c_str());
    if (dirp == NULL)
        return (0);
    int i = 0;
    while ((dp = readdir(dirp)) != NULL)
    {
        if (dp->d_name[0] == '.')
            continue;
        if (dp->d_type == DT_DIR)
        {
            i++;
            std::string tmp = dir + "" + dp->d_name;
            // std::cout << tmp << "  " << str << "\n";
            if (tmp.find(str) != std::string::npos)
                return (1);
        }
        else if(dp->d_type == DT_REG)
        {
            std::string tmp = dir + "" + dp->d_name;
            // std::cout << tmp << "  " << str << "\n";
            if (tmp.find(str) != std::string::npos)
                return (2);
        }
    }
    closedir(dirp);
    if(str.find(".") == std::string::npos)
    {
        dirp = opendir(str.c_str());
        if (dirp == NULL)
            return (0);
    }
    else
        return (0);
    return (2);
}


std::string getBody(std::string path)
{
    std::string body;
    std::ifstream file(path);
    if (file.is_open())
    {
        std::string line;
        while (getline(file, line))
        {
            body += line;
            body += "\n";
        }
        file.close();
    }
    return body;
}

Server::Server(Parsing *p)
{
    std::map<std::string, std::string> stor;
    std::memset((char *)&this->add, 0, sizeof(this->add));
    int server_fd;  // socket descriptor, an integer!
    int new_socket; // conection establish btw client & server
    int valrecv;    // communication part
    struct sockaddr_in add;
    struct sockaddr_in client;
    socklen_t size_client = sizeof(client); // socklen_t size of adress

    // ** CREATE SOCKET**/
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
    {
        std::cerr << "socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    ///Set Socket
    //int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_length);
    //it helps in reuse of address and port 
    int option = 1;
    if (setsockopt(server_fd, SOL_SOCKET,  SO_REUSEPORT, &option, sizeof(option)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    // bind an IP add and a port to a  socket
    //   p->GetServerMap().find()
    std::map<int, std::multimap<std::string, std::string> > tmp = p->GetServerMap();
    std::multimap<int, std::multimap<std::string, std::string> > loc = p->Getloc_map();
    std::multimap<std::string, std::string> mtmp = tmp[1];

    add.sin_port = htons(std::stoi(mtmp.find("listen")->second));
    add.sin_family = AF_INET;
    if (mtmp.find("server_addr")->second.c_str() != NULL)
        add.sin_addr.s_addr = inet_addr(mtmp.find("server_addr")->second.c_str());
    else
        add.sin_addr.s_addr = INADDR_ANY;

    memset(add.sin_zero, '\0', sizeof add.sin_zero); // why help to pad from sockaddr_in to sockaddr

    // Forcefully attaching socket to the PORT
    if (bind(server_fd, (struct sockaddr *)&add, sizeof(add)) < 0)
    {
        perror("Bind");
        std::cerr << "Bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    // wait for an incoming connection
    if (listen(server_fd, SOMAXCONN)) // SOMAXCONN is the maximum number of pending connections that can be queued up before connections are refused.
    {
        std::cerr << "Listening failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    // std::multimap<int, std::multimap<std::string, std::string>  >::iterator it0;
    // std::multimap<std::string, std::string>::iterator it;
    // for(it0 = loc.begin(); it0 != loc.end(); ++it0)
    // {
    //     // std::cout << "----------------------------------\n";
    //     std::cout << "for server " << it0->first << std::endl;
    //     for(it = it0->second.begin(); it != it0->second.end(); ++it)
    //     {
    //         std::cout << it->first << " ==> " << it->second << std::endl;
    //     }
    // }
    while (1)
    {
        std::cout << "\t\t\t Listening " << mtmp.find("listen")->second << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client,
                                 &size_client)) < 0)
        {
            std::cerr << "acceptance failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        char buffer[30000] = {0};
        valrecv = recv(new_socket, buffer, 30000, 0);

        std::string tmp;
        std::string body = "";
        std::string someString(buffer);
        //  show the request
        // std::cout << someString << std::endl;
        std::stringstream out;
        // out << someString;
        int len = nbr_lines(someString);
        std::string line1;
        std::string status = "200 OK";
        std::string tmp2;
        std::string Content;
        int t = 0;
        int i = 0;
        int lenght = 0;
        while (i < len ) 
        {       
            line1 = Those_lines(someString, i, len);
            if (line1.find_first_of(":") != std::string::npos && t == 1)
            {
                tmp = line1.substr(0, line1.find_first_of(":"));
                tmp2 = line1.substr(line1.find_first_of(":") + 1);
                stor[tmp] = tmp2;
            }
            else if ( t == 0)
            {
                tmp = line1.substr(0, line1.find_first_of(" "));
                tmp2 = line1.substr(line1.find_first_of(" ") + 1);
                stor[tmp] = "webpage" + tmp2.substr(0, tmp2.find_first_of(" "));
                t++;
            }
            else
            {
                t++;
                Content += line1;
            }
            i++;
        }
        // std::map< std::string, std::string>::iterator tt;
        // for(tt  = stor.begin(); tt != stor.end(); tt++)
        // {
        //     std::cout << "stror[" << tt->first << "] \t=> [" << tt->second << "]\n" ;
        // } 
        // std::cout << "content ===> " << Content << std::endl;
        if (stor.find("GET") != stor.end())
        {
            int dir = 0;
            std::multimap<int, std::multimap<std::string, std::string> > tmp = p->Getloc_map();
            std::multimap<std::string, std::string> mtmp = tmp.find(1)->second;
            std::string path = "webpage" + mtmp.find("location")->second;
            // std::cout << stor.find("GET")->second.substr(0,stor.find("GET")->second.find_last_not_of("/") << std::endl;
            if (stor.find("GET")->second == path)
            {
                body = getBody(stor.find("GET")->second + "/index.html");
                lenght = body.size();
            }
            else if ((dir =check_dir(path, stor.find("GET")->second)))
            {
                if(dir == 1)
                {
                    body = getBody(stor.find("GET")->second + "/index.html");
                    lenght = body.size();
                }
                else 
                {
                    body = getBody(stor.find("GET")->second);
                    lenght = body.size();
                }
            }
            else
            {
                body = getBody("webpage/errors/404.html");
                lenght = body.size();
                status = "404 Not Found";
            }
        }
        else if (stor.find("POST") != stor.end())
        {
            std::multimap<int, std::multimap<std::string, std::string> > tmp = p->Getloc_map();
            std::multimap<std::string, std::string> mtmp = tmp.find(1)->second;
            std::multimap<std::string, std::string>::iterator it;
            if (mtmp.find("http_methods")->second.find("POST") != std::string::npos)
            {
                std::string path = "webpage" + mtmp.find("location")->second;
                if (stor.find("POST")->second == path)
                {
                    body = getBody(stor.find("POST")->second + "/index.html");
                    lenght = body.size();
                    int k = 0;
                    for (it = mtmp.begin(); it != mtmp.end(); ++it)
                    {
                        // if (k == 1 || mtmp.find("upload") != mtmp.end())
                        // {
                        //     if( mtmp.find("upload") != mtmp.end()  && it->second )
                        //         k = 1;
                        //     if (  mtmp.find("upload_location") != mtmp.end())
                        //     {
                        //         if ( it->second == std::string::npos)
                        //             std::cout << "Saved uploaded files in [/Users/zdnaya/Downloads]\n";
                        //         else
                        //         {
                        //             std::cout << "\033[3;47;35m Save the upload\033[0m\t\n";
                        //         }
                        //     }
                        // }
                    }
                }
                else if (stor.find("POST")->second != path)
                {
                    body = getBody("webpage/errors/404.html");
                    lenght = body.size();
                    status = "404 not found";
                }
                else
                {
                    std::cout << "["+Content +"]" << std::endl;
                    body = getBody("webpage/");
                    lenght = body.size();
                    status = "200 Ok";
                }
            }
            else
            {
                body = getBody("webpage/errors/405.html");
                lenght = body.size();
                status = "405 Not Allowed";
            }
        }
        std::string header = "HTTP/1.1 " + status + "\nContent-Type: text/html\nContent-Length: " + std::to_string(lenght) + "\n\n" + body;
        write(new_socket, header.c_str(), strlen(header.c_str()));
        close(new_socket);
        stor.clear();
    }
    close(server_fd);
}

int Server::creatSocket_fd()
{
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket_fd < 0)
        error_msg("Error: you can't create socket");
    return (this->socket_fd);
}

void Server::set_strructAddr(struct sockaddr_in add)
{
    add.sin_port = htons(5003);
    add.sin_family = AF_INET;
    add.sin_addr.s_addr = htonl(INADDR_ANY);
}

int Server::get_sizeofAdd()
{
    return (sizeof(this->add)); // socklen_t size of adress
}

void Server::bind_socket(struct sockaddr_in add)
{
    if (bind(this->socket_fd, (struct sockaddr *)&add, get_sizeofAdd()) < 0)
        error_msg("Error: Binding socket failed");
}

void Server::listen_socket(int sockket_fd)
{
    if (listen(socket_fd, SOMAXCONN) < 0) // SOMAXCONN is the maximum number of pending connections that can be queued up before connections are refused.
    {
        std::cerr << "Listening failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::accept_socket(int socket_fd, struct sockaddr_in add)
{
    socklen_t size_add = sizeof(add);

    if ((new_socket = accept(socket_fd, (struct sockaddr *)&add, &size_add)) < 0)
    {
        std::cerr << "acceptance failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Server::~Server()
{
}