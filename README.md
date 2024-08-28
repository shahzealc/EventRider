# EventRiders

EventRiders is a local events and activities finder platform built using the C++ and JS. It allows users to discover, create, and manage events in their locality. The platform is designed to provide a seamless user experience, featuring user authentication, event listings, and booking functionalities.

## Features

1. **User Authentication**:
   - Users can register, log in, and log out securely using JSON Web Tokens (JWT).
   - Authentication middleware ensures protected routes are accessible only to authenticated users.

2. **Event Listings**:
   - Events are displayed in a card layout with details such as event name, city, type, fees, date, and time.
   - Each card has options to update or delete the event, provided the user has the appropriate permissions.

3. **Event Creation and Management**:
   - Users can create new events using a dedicated form accessible through the "Add Event" button.
   - Existing events can be updated or deleted by their creators.
   - Events data is fetched from a backend API and displayed dynamically on the front end.

4. **Bookings Management**:
   - Users can view their bookings on a dedicated page.
   - Functionality to manage bookings is integrated for a seamless user experience.

5. **Responsive Design**:
   - The application is designed with responsiveness in mind, ensuring a good user experience across different devices.


## Technologies and Tools Used

- **Frontend**:
  - HTML, CSS 
  - JavaScript
- **Backend**:
  - C++
  - Crow 
- **Database**:
  - MySQLite
- **Authentication**:
  - JSON Web Tokens (JWT) for secure authentication

## API Endpoints

### Authentication Endpoints

- **POST /login**: Authenticates a user and returns a JWT token if the credentials are valid.
  - **Request Body**: JSON containing `email` and `password`.
  - **Response**: JSON containing `message`, `token`, and `id` on successful login.

- **POST /register**: Registers a new user.
  - **Request Body**: JSON containing `name`, `email`, `password`, and `city`.
  - **Response**: JSON containing `message` indicating the success or failure of registration.

### User Endpoints

- **GET /user**: Retrieves user information based on user ID.
  - **Query Parameters**: `id` (User ID).
  - **Response**: JSON containing `name`, `email`, and a `message` indicating the success or failure of the request.

### Event Endpoints

- **POST /addevent**: Creates a new event.
  - **Request Body**: JSON containing `name`, `address`, `city`, `type`, `description`, `fees`, `max_seat`, `date`, `time`, `image` (base64 encoded), and `filename`.
  - **Response**: JSON indicating the success or failure of the event creation.

- **GET /events**: Retrieves events based on various query parameters.
  - **Query Parameters** (optional):
    - `location` (Event location)
    - `name` (Event name)
    - `creator` (Creator ID)
    - `type` (Event type)
    - `search` (Search term)
    - `eventid` (Event ID)
  - **Response**: JSON containing event details matching the query parameters.

- **DELETE /events**: Deletes an event based on the event ID.
  - **Query Parameters**: `id` (Event ID).
  - **Response**: HTTP status code indicating the success or failure of the event deletion.

- **PUT /events**: Updates an existing event.
  - **Request Body**: JSON containing `eventid`, `name`, `address`, `city`, `type`, `description`, `fees`, `max_seat`, `date`, and `time`.
  - **Response**: JSON containing `message` indicating the success or failure of the update.

### Booking Endpoints

- **POST /bookevent**: Books an event for a user.
  - **Request Body**: JSON containing `eventid`, `userid`, and `seats`.
  - **Response**: JSON containing `message` indicating the success or failure of the booking.

- **GET /alreadybooked**: Checks if a user has already booked an event.
  - **Query Parameters**: `eventid` (Event ID), `userid` (User ID).
  - **Response**: JSON containing `booked` (boolean).

- **GET /bookings**: Retrieves all bookings for the authenticated user.
  - **Response**: JSON containing booking details for the user.

### Utility Endpoints

- **GET /checktoken**: Verifies if the user is authenticated.
  - **Response**: User ID if authenticated.

### General

- Any undefined routes will return a 404 response with the message "Wrong Route".
